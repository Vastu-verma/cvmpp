#pragma once

#include <sstream>
#include <string>
#include <variant>
#include <vector>

namespace cvm {

using Constant = std::variant<double, bool, std::string>;

enum class OpCode {
  Constant,
  LoadGlobal,
  StoreGlobal,
  Add,
  Subtract,
  Multiply,
  Divide,
  Negate,
  Not,
  Equal,
  NotEqual,
  Greater,
  GreaterEqual,
  Less,
  LessEqual,
  Jump,
  JumpIfFalse,
  Print,
  ReadInput,
  Pop,
  Halt
};

struct Instruction {
  OpCode opcode;
  int operand = -1;
};

struct Chunk {
  std::vector<Instruction> code;
  std::vector<Constant> constants;

  int addConstant(Constant constant) {
    constants.push_back(std::move(constant));
    return static_cast<int>(constants.size()) - 1;
  }
};

std::string opcodeToString(OpCode opcode);
std::string constantToString(const Constant& constant);
std::string disassemble(const Chunk& chunk);

}  // namespace cvm
