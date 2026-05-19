#pragma once

#include <vector>

#include "cvm/Ast.h"
#include "cvm/Bytecode.h"

namespace cvm {

class Compiler {
 public:
  Chunk compile(const std::vector<StmtPtr>& program);

 private:
  void emit(OpCode opcode, int operand = -1);
  int emitJump(OpCode opcode);
  void patchJump(int instructionIndex);
  void compileStmt(const Stmt& stmt);
  void compileExpr(const Expr& expr);
  int addNameConstant(const std::string& name);

  Chunk chunk_;
};

}  // namespace cvm
