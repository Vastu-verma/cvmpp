#include "cvm/Bytecode.h"

#include <iomanip>

namespace cvm {

std::string opcodeToString(OpCode opcode) {
  switch (opcode) {
    case OpCode::Constant: return "OP_CONSTANT";
    case OpCode::LoadGlobal: return "OP_LOAD_GLOBAL";
    case OpCode::StoreGlobal: return "OP_STORE_GLOBAL";
    case OpCode::Add: return "OP_ADD";
    case OpCode::Subtract: return "OP_SUBTRACT";
    case OpCode::Multiply: return "OP_MULTIPLY";
    case OpCode::Divide: return "OP_DIVIDE";
    case OpCode::Negate: return "OP_NEGATE";
    case OpCode::Not: return "OP_NOT";
    case OpCode::Equal: return "OP_EQUAL";
    case OpCode::NotEqual: return "OP_NOT_EQUAL";
    case OpCode::Greater: return "OP_GREATER";
    case OpCode::GreaterEqual: return "OP_GREATER_EQUAL";
    case OpCode::Less: return "OP_LESS";
    case OpCode::LessEqual: return "OP_LESS_EQUAL";
    case OpCode::Jump: return "OP_JUMP";
    case OpCode::JumpIfFalse: return "OP_JUMP_IF_FALSE";
    case OpCode::Print: return "OP_PRINT";
    case OpCode::Pop: return "OP_POP";
    case OpCode::Halt: return "OP_HALT";
  }
  return "OP_UNKNOWN";
}

std::string constantToString(const Constant& constant) {
  if (std::holds_alternative<double>(constant)) {
    std::ostringstream out;
    out << std::get<double>(constant);
    return out.str();
  }
  if (std::holds_alternative<bool>(constant)) {
    return std::get<bool>(constant) ? "true" : "false";
  }
  return '"' + std::get<std::string>(constant) + '"';
}

std::string disassemble(const Chunk& chunk) {
  std::ostringstream out;
  out << "== Bytecode ==\n";
  for (std::size_t i = 0; i < chunk.code.size(); ++i) {
    const auto& instruction = chunk.code[i];
    out << std::setw(4) << i << "  " << opcodeToString(instruction.opcode);
    if (instruction.operand >= 0) {
      out << " " << instruction.operand;
      if (instruction.opcode == OpCode::Constant ||
          instruction.opcode == OpCode::LoadGlobal ||
          instruction.opcode == OpCode::StoreGlobal) {
        out << " (" << constantToString(chunk.constants[instruction.operand])
            << ")";
      }
    }
    out << "\n";
  }
  return out.str();
}

}  // namespace cvm
