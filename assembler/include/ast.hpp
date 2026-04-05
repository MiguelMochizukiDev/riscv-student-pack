#pragma once

#include <cstdint>
#include <string>
#include <variant>
#include <vector>

enum class TokenKind {
	Identifier,
	Directive,
	Register,
	Integer,
	Comma,
	Colon,
	LParen,
	RParen,
	EndOfLine
};

struct Token {
	TokenKind kind{};
	std::string lexeme;
	int line = 0;
	int column = 0;
};

struct SourceLine {
	int lineNumber = 0;
	std::vector<Token> tokens;
};

enum class OperandKind {
	Register,
	Immediate,
	Symbol,
	Memory
};

struct MemoryOperand {
	int32_t offset = 0;
	int baseRegister = 0;
};

struct Operand {
	OperandKind kind{};
	std::variant<int32_t, std::string, MemoryOperand> value;
};

struct InstructionNode {
	std::string mnemonic;
	std::vector<Operand> operands;
	int line = 0;
};

struct DirectiveNode {
	std::string name;
	std::vector<Operand> operands;
	int line = 0;
};

using Statement = std::variant<std::monostate, InstructionNode, DirectiveNode>;

struct ParsedLine {
	std::string label;
	Statement statement;
	int line = 0;
};

struct Program {
	std::vector<ParsedLine> lines;
};

struct AssemblyImage {
	uint32_t baseAddress = 0;
	uint32_t entryPoint = 0;
	std::vector<uint8_t> text;
};
