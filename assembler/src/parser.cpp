#include "../include/parser.hpp"
#include <climits>
#include <stdexcept>
#include <unordered_map>

Program Parser::parse(const std::vector<SourceLine> &lines) const {
	Program program;
	program.lines.reserve(lines.size());
	for (const auto &line : lines)
		program.lines.push_back(parseLine(line));
	return program;
}

ParsedLine Parser::parseLine(const SourceLine &line) const {
	ParsedLine parsed;
	parsed.line = line.lineNumber;
	const auto &tokens = line.tokens;
	if (tokens.empty())
		return parsed;

	size_t index = 0;
	if (tokens.size() >= 2 && tokens[0].kind == TokenKind::Identifier && tokens[1].kind == TokenKind::Colon) {
		parsed.label = tokens[0].lexeme;
		index = 2;
	}

	if (index >= tokens.size())
		return parsed;

	if (tokens[index].kind == TokenKind::Directive) {
		parsed.statement = parseDirective(tokens, index, line.lineNumber);
		return parsed;
	}

	parsed.statement = parseInstruction(tokens, index, line.lineNumber);
	return parsed;
}

DirectiveNode Parser::parseDirective(const std::vector<Token> &tokens, size_t &index, int lineNumber) const {
	DirectiveNode directive;
	directive.line = lineNumber;
	directive.name = tokens[index].lexeme;
	index++;

	while (index < tokens.size()) {
		directive.operands.push_back(parseOperand(tokens, index, lineNumber));
		if (index < tokens.size()) {
			if (tokens[index].kind != TokenKind::Comma)
				throw std::runtime_error("Expected ',' after directive operand at line " + std::to_string(lineNumber));
			index++;
		}
	}
	return directive;
}

InstructionNode Parser::parseInstruction(const std::vector<Token> &tokens, size_t &index, int lineNumber) const {
	if (tokens[index].kind != TokenKind::Identifier)
		throw std::runtime_error("Expected instruction mnemonic at line " + std::to_string(lineNumber));

	InstructionNode instruction;
	instruction.line = lineNumber;
	instruction.mnemonic = tokens[index].lexeme;
	index++;

	while (index < tokens.size()) {
		instruction.operands.push_back(parseOperand(tokens, index, lineNumber));
		if (index < tokens.size()) {
			if (tokens[index].kind != TokenKind::Comma)
				throw std::runtime_error("Expected ',' after instruction operand at line " + std::to_string(lineNumber));
			index++;
		}
	}
	return instruction;
}

Operand Parser::parseOperand(const std::vector<Token> &tokens, size_t &index, int lineNumber) const {
	if (index >= tokens.size())
		throw std::runtime_error("Unexpected end of line while parsing operand");

	const Token &token = tokens[index];

	if (token.kind == TokenKind::Register) {
		index++;
		return Operand{OperandKind::Register, static_cast<int32_t>(parseRegisterIndex(token.lexeme))};
	}

	if (token.kind == TokenKind::LParen) {
		if (index + 2 >= tokens.size() || tokens[index + 1].kind != TokenKind::Register || tokens[index + 2].kind != TokenKind::RParen)
			throw std::runtime_error("Malformed memory operand at line " + std::to_string(lineNumber));
		MemoryOperand mem{0, parseRegisterIndex(tokens[index + 1].lexeme)};
		index += 3;
		return Operand{OperandKind::Memory, mem};
	}

	if (token.kind == TokenKind::Integer) {
		int32_t immediate = parseInteger(token.lexeme);
		if (index + 3 < tokens.size() && tokens[index + 1].kind == TokenKind::LParen && tokens[index + 2].kind == TokenKind::Register &&
		    tokens[index + 3].kind == TokenKind::RParen) {
			MemoryOperand mem{immediate, parseRegisterIndex(tokens[index + 2].lexeme)};
			index += 4;
			return Operand{OperandKind::Memory, mem};
		}
		index++;
		return Operand{OperandKind::Immediate, immediate};
	}

	if (token.kind == TokenKind::Identifier) {
		index++;
		return Operand{OperandKind::Symbol, token.lexeme};
	}

	throw std::runtime_error("Unexpected operand token at line " + std::to_string(lineNumber));
}

int Parser::parseRegisterIndex(const std::string &name) {
	if (name.size() >= 2 && name[0] == 'x')
		return std::stoi(name.substr(1));

	static const std::unordered_map<std::string, int> abiMap = {
	    {"zero", 0}, {"ra", 1}, {"sp", 2}, {"gp", 3}, {"tp", 4}, {"t0", 5}, {"t1", 6}, {"t2", 7}, {"s0", 8}, {"fp", 8}, {"s1", 9}, {"a0", 10}, {"a1", 11}, {"a2", 12}, {"a3", 13}, {"a4", 14}, {"a5", 15}, {"a6", 16}, {"a7", 17}, {"s2", 18}, {"s3", 19}, {"s4", 20}, {"s5", 21}, {"s6", 22}, {"s7", 23}, {"s8", 24}, {"s9", 25}, {"s10", 26}, {"s11", 27}, {"t3", 28}, {"t4", 29}, {"t5", 30}, {"t6", 31}};
	const auto it = abiMap.find(name);
	if (it == abiMap.end())
		throw std::runtime_error("Unknown register name: " + name);
	return it->second;
}

int32_t Parser::parseInteger(const std::string &text) {
	long long value = std::stoll(text, nullptr, 0);
	if (value < INT32_MIN || value > INT32_MAX)
		throw std::runtime_error("Immediate out of 32-bit range: " + text);
	return static_cast<int32_t>(value);
}
