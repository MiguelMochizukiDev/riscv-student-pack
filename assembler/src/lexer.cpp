#include "../include/lexer.hpp"
#include <cctype>
#include <stdexcept>

bool Lexer::isIdentifierStart(char c) {
	return static_cast<bool>(std::isalpha(static_cast<unsigned char>(c))) || c == '_' || c == '.';
}

bool Lexer::isIdentifierChar(char c) {
	return static_cast<bool>(std::isalnum(static_cast<unsigned char>(c))) || c == '_' || c == '.';
}

bool Lexer::isRegisterName(const std::string &text) {
	if (text.size() >= 2 && text[0] == 'x') {
		for (size_t i = 1; i < text.size(); ++i) {
			if (!std::isdigit(static_cast<unsigned char>(text[i])))
				return false;
		}
		int idx = std::stoi(text.substr(1));
		return idx >= 0 && idx <= 31;
	}

	static const char *abiNames[] = {
	    "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2", "s0", "fp", "s1", "a0", "a1", "a2", "a3", "a4",
	    "a5", "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"};
	for (const auto *name : abiNames) {
		if (text == name)
			return true;
	}
	return false;
}

bool Lexer::isNumberLiteral(const std::string &text) {
	if (text.empty())
		return false;

	size_t start = 0;
	if (text[0] == '+' || text[0] == '-') {
		if (text.size() == 1)
			return false;
		start = 1;
	}

	if (start + 2 <= text.size() && text[start] == '0' && (text[start + 1] == 'x' || text[start + 1] == 'X')) {
		if (start + 2 == text.size())
			return false;
		for (size_t i = start + 2; i < text.size(); ++i) {
			if (!std::isxdigit(static_cast<unsigned char>(text[i])))
				return false;
		}
		return true;
	}

	for (size_t i = start; i < text.size(); ++i) {
		if (!std::isdigit(static_cast<unsigned char>(text[i])))
			return false;
	}
	return true;
}

Token Lexer::classifyWord(const std::string &word, int line, int column) {
	Token token;
	token.lexeme = word;
	token.line = line;
	token.column = column;

	if (!word.empty() && word[0] == '.') {
		token.kind = TokenKind::Directive;
		return token;
	}
	if (isRegisterName(word)) {
		token.kind = TokenKind::Register;
		return token;
	}
	if (isNumberLiteral(word)) {
		token.kind = TokenKind::Integer;
		return token;
	}
	token.kind = TokenKind::Identifier;
	return token;
}

std::vector<SourceLine> Lexer::tokenize(const std::string &source) const {
	std::vector<SourceLine> result;
	std::string current;
	std::vector<Token> tokens;
	int line = 1;
	int column = 1;
	int tokenStart = 1;

	auto flushToken = [&](int currentLine) {
		if (!current.empty()) {
			tokens.push_back(classifyWord(current, currentLine, tokenStart));
			current.clear();
		}
	};

	for (size_t i = 0; i <= source.size(); ++i) {
		char c = i < source.size() ? source[i] : '\n';

		if (c == '#') {
			flushToken(line);
			while (i < source.size() && source[i] != '\n')
				++i;
			c = i < source.size() ? source[i] : '\n';
		}

		if (c == '\n') {
			flushToken(line);
			result.push_back(SourceLine{line, tokens});
			tokens.clear();
			line++;
			column = 1;
			continue;
		}

		if (std::isspace(static_cast<unsigned char>(c))) {
			flushToken(line);
			column++;
			continue;
		}

		if (c == ',' || c == ':' || c == '(' || c == ')') {
			flushToken(line);
			Token token;
			token.lexeme = std::string(1, c);
			token.line = line;
			token.column = column;
			switch (c) {
			case ',':
				token.kind = TokenKind::Comma;
				break;
			case ':':
				token.kind = TokenKind::Colon;
				break;
			case '(':
				token.kind = TokenKind::LParen;
				break;
			case ')':
				token.kind = TokenKind::RParen;
				break;
			default:
				throw std::runtime_error("Lexer internal tokenization error");
			}
			tokens.push_back(token);
			column++;
			continue;
		}

		if (current.empty())
			tokenStart = column;
		if (!isIdentifierChar(c) && c != '+' && c != '-')
			throw std::runtime_error("Unexpected character in input at line " + std::to_string(line));
		current.push_back(c);
		column++;
	}

	return result;
}
