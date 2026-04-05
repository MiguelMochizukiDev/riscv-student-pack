#include "../include/assembler.hpp"
#include "../include/elf_writer.hpp"
#include "../include/mnemonic_handler.hpp"
#include <fstream>
#include <stdexcept>

static constexpr uint32_t DEFAULT_BASE_ADDRESS = 0x00000000;

class SymbolResolver : public ISymbolResolver {
      public:
	explicit SymbolResolver(const Assembler::SymbolTable &symbols) : symbols(symbols) {}

	bool tryResolve(const std::string &name, uint32_t &value) const override {
		auto it = symbols.values.find(name);
		if (it == symbols.values.end())
			return false;
		value = it->second;
		return true;
	}

      private:
	const Assembler::SymbolTable &symbols;
};

static void appendWordLE(std::vector<uint8_t> &target, uint32_t value) {
	target.push_back(static_cast<uint8_t>(value & 0xFF));
	target.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
	target.push_back(static_cast<uint8_t>((value >> 16) & 0xFF));
	target.push_back(static_cast<uint8_t>((value >> 24) & 0xFF));
}

Assembler::Assembler() = default;

void Assembler::registerHandler(std::unique_ptr<MnemonicHandler> handler) {
	handlers.push_back(std::move(handler));
}

Assembler::SymbolTable Assembler::firstPass(const Program &program, uint32_t baseAddress) const {
	SymbolTable symbols;
	uint32_t currentAddress = baseAddress;

	for (const auto &line : program.lines) {
		if (!line.label.empty()) {
			if (symbols.values.count(line.label) != 0)
				throw std::runtime_error("Duplicate label: " + line.label + " at line " + std::to_string(line.line));
			symbols.values[line.label] = currentAddress;
		}

		if (std::holds_alternative<DirectiveNode>(line.statement)) {
			const auto &directive = std::get<DirectiveNode>(line.statement);
			currentAddress += estimateDirectiveSize(directive);
		} else if (std::holds_alternative<InstructionNode>(line.statement)) {
			currentAddress += 4;
		}
	}

	return symbols;
}

AssemblyImage Assembler::secondPass(const Program &program, const SymbolTable &symbols, uint32_t baseAddress) const {
	AssemblyImage image;
	image.baseAddress = baseAddress;
	uint32_t currentAddress = baseAddress;
	SymbolResolver resolver(symbols);

	for (const auto &line : program.lines) {
		if (std::holds_alternative<DirectiveNode>(line.statement)) {
			const auto &directive = std::get<DirectiveNode>(line.statement);
			if (directive.name == ".text") {
				continue;
			}
			if (directive.name == ".word") {
				for (const auto &operand : directive.operands) {
					uint32_t value = resolveValue(operand, symbols);
					appendWordLE(image.text, value);
					currentAddress += 4;
				}
				continue;
			}
			throw std::runtime_error("Unsupported directive in pass2: " + directive.name + " at line " + std::to_string(directive.line));
		}

		if (std::holds_alternative<InstructionNode>(line.statement)) {
			const auto &instruction = std::get<InstructionNode>(line.statement);
			EncodeContext context{resolver, currentAddress};
			uint32_t encoded = 0;
			bool handled = false;
			for (const auto &handler : handlers) {
				if (handler->handle(instruction, context, encoded)) {
					handled = true;
					break;
				}
			}
			if (!handled) {
				throw std::runtime_error("Unsupported instruction: " + instruction.mnemonic + " at line " + std::to_string(instruction.line));
			}
			appendWordLE(image.text, encoded);
			currentAddress += 4;
		}
	}

	auto entry = symbols.values.find("_start");
	image.entryPoint = (entry != symbols.values.end()) ? entry->second : baseAddress;
	return image;
}

uint32_t Assembler::estimateDirectiveSize(const DirectiveNode &directive) {
	if (directive.name == ".text")
		return 0;
	if (directive.name == ".word")
		return static_cast<uint32_t>(directive.operands.size() * 4);
	throw std::runtime_error("Unsupported directive: " + directive.name + " at line " + std::to_string(directive.line));
}

uint32_t Assembler::resolveValue(const Operand &operand, const SymbolTable &symbols) {
	if (operand.kind == OperandKind::Immediate)
		return static_cast<uint32_t>(std::get<int32_t>(operand.value));
	if (operand.kind == OperandKind::Symbol) {
		auto it = symbols.values.find(std::get<std::string>(operand.value));
		if (it == symbols.values.end())
			throw std::runtime_error("Undefined symbol: " + std::get<std::string>(operand.value));
		return it->second;
	}
	throw std::runtime_error("Only immediates and symbols are supported in .word");
}
AssemblyImage Assembler::assemble(const std::string &source) const {
	Lexer lexer;
	Parser parser;

	auto tokenLines = lexer.tokenize(source);
	Program program = parser.parse(tokenLines);
	SymbolTable symbols = firstPass(program, DEFAULT_BASE_ADDRESS);
	return secondPass(program, symbols, DEFAULT_BASE_ADDRESS);
}

void Assembler::assembleFile(const std::string &inputPath, const std::string &outputPath) const {
	std::ifstream in(inputPath);
	if (!in)
		throw std::runtime_error("Unable to open input source file: " + inputPath);

	std::string source((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
	AssemblyImage image = assemble(source);
	ELFWriter writer;
	writer.writeExecutable(outputPath, image);
}
