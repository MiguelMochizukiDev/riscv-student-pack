#pragma once

#include "ast.hpp"
#include <vector>

class Parser {
      public:
	Program parse(const std::vector<SourceLine> &lines) const;

      private:
	ParsedLine parseLine(const SourceLine &line) const;
	DirectiveNode parseDirective(const std::vector<Token> &tokens, size_t &index, int lineNumber) const;
	InstructionNode parseInstruction(const std::vector<Token> &tokens, size_t &index, int lineNumber) const;
	Operand parseOperand(const std::vector<Token> &tokens, size_t &index, int lineNumber) const;
	static int parseRegisterIndex(const std::string &name);
	static int32_t parseInteger(const std::string &text);
};
