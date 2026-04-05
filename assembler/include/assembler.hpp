#pragma once

#include "ast.hpp"
#include "lexer.hpp"
#include "mnemonic_handler.hpp"
#include "parser.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class Assembler {
      public:
	struct SymbolTable {
		std::unordered_map<std::string, uint32_t> values;
	};

	Assembler();

	void registerHandler(std::unique_ptr<MnemonicHandler> handler);

	AssemblyImage assemble(const std::string &source) const;
	void assembleFile(const std::string &inputPath, const std::string &outputPath) const;

      private:
	std::vector<std::unique_ptr<MnemonicHandler>> handlers;

	SymbolTable firstPass(const Program &program, uint32_t baseAddress) const;
	AssemblyImage secondPass(const Program &program, const SymbolTable &symbols, uint32_t baseAddress) const;

	static uint32_t estimateDirectiveSize(const DirectiveNode &directive);
	static uint32_t resolveValue(const Operand &operand, const SymbolTable &symbols);
};
