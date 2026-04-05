#pragma once

#include "ast.hpp"
#include <string>

class ELFWriter {
      public:
	void writeExecutable(const std::string &outputPath, const AssemblyImage &image) const;
};
