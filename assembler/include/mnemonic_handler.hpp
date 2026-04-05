#pragma once

#include "ast.hpp"
#include <cstdint>
#include <string>

class ISymbolResolver {
      public:
	virtual ~ISymbolResolver() = default;
	virtual bool tryResolve(const std::string &name, uint32_t &value) const = 0;
};

struct EncodeContext {
	const ISymbolResolver &resolver;
	uint32_t currentAddress = 0;
};

class MnemonicHandler {
      public:
	virtual ~MnemonicHandler() = default;
	virtual bool handle(const InstructionNode &instruction, const EncodeContext &context, uint32_t &encodedInstruction) const = 0;
};

class RV32IMnemonicHandler : public MnemonicHandler {
      public:
	bool handle(const InstructionNode &instruction, const EncodeContext &context, uint32_t &encodedInstruction) const override;
};
