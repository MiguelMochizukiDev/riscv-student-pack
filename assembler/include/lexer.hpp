#pragma once

#include "ast.hpp"
#include <string>
#include <vector>

class Lexer {
      public:
	std::vector<SourceLine> tokenize(const std::string &source) const;

      private:
	static bool isIdentifierStart(char c);
	static bool isIdentifierChar(char c);
	static bool isRegisterName(const std::string &text);
	static bool isNumberLiteral(const std::string &text);
	static Token classifyWord(const std::string &word, int line, int column);
};
