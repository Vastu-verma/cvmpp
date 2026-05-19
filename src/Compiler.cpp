#include "cvm/Compiler.h"

#include <stdexcept>

namespace cvm {

Chunk Compiler::compile(const std::vector<StmtPtr>& program) {
  chunk_ = Chunk{};
  for (const auto& stmt : program) compileStmt(*stmt);
  emit(OpCode::Halt);
  return chunk_;
}

void Compiler::emit(OpCode opcode, int operand) {
  chunk_.code.push_back({opcode, operand});
}

int Compiler::emitJump(OpCode opcode) {
  emit(opcode, -1);
  return static_cast<int>(chunk_.code.size()) - 1;
}

void Compiler::patchJump(int instructionIndex) {
  chunk_.code.at(static_cast<std::size_t>(instructionIndex)).operand =
      static_cast<int>(chunk_.code.size());
}

void Compiler::compileStmt(const Stmt& stmt) {
  if (const auto* letStmt = dynamic_cast<const LetStmt*>(&stmt)) {
    compileExpr(*letStmt->initializer);
    emit(OpCode::StoreGlobal, addNameConstant(letStmt->name.lexeme));
    emit(OpCode::Pop);
    return;
  }
  if (const auto* printStmt = dynamic_cast<const PrintStmt*>(&stmt)) {
    compileExpr(*printStmt->expression);
    emit(OpCode::Print);
    return;
  }
  if (const auto* exprStmt = dynamic_cast<const ExprStmt*>(&stmt)) {
    compileExpr(*exprStmt->expression);
    emit(OpCode::Pop);
    return;
  }
  if (const auto* blockStmt = dynamic_cast<const BlockStmt*>(&stmt)) {
    for (const auto& child : blockStmt->statements) compileStmt(*child);
    return;
  }
  if (const auto* ifStmt = dynamic_cast<const IfStmt*>(&stmt)) {
    compileExpr(*ifStmt->condition);
    int jumpToElse = emitJump(OpCode::JumpIfFalse);
    emit(OpCode::Pop);
    compileStmt(*ifStmt->thenBranch);
    int jumpToEnd = emitJump(OpCode::Jump);
    patchJump(jumpToElse);
    emit(OpCode::Pop);
    if (ifStmt->elseBranch) compileStmt(*ifStmt->elseBranch);
    patchJump(jumpToEnd);
    return;
  }
  if (const auto* whileStmt = dynamic_cast<const WhileStmt*>(&stmt)) {
    int loopStart = static_cast<int>(chunk_.code.size());
    compileExpr(*whileStmt->condition);
    int exitJump = emitJump(OpCode::JumpIfFalse);
    emit(OpCode::Pop);
    compileStmt(*whileStmt->body);
    emit(OpCode::Jump, loopStart);
    patchJump(exitJump);
    emit(OpCode::Pop);
    return;
  }
  throw std::runtime_error("Compiler error: unsupported statement node.");
}

void Compiler::compileExpr(const Expr& expr) {
  if (const auto* literal = dynamic_cast<const LiteralExpr*>(&expr)) {
    if (literal->value == "true") {
      emit(OpCode::Constant, chunk_.addConstant(true));
    } else if (literal->value == "false") {
      emit(OpCode::Constant, chunk_.addConstant(false));
    } else {
      emit(OpCode::Constant, chunk_.addConstant(std::stod(literal->value)));
    }
    return;
  }
  if (const auto* variable = dynamic_cast<const VariableExpr*>(&expr)) {
    emit(OpCode::LoadGlobal, addNameConstant(variable->name.lexeme));
    return;
  }
  if (const auto* assign = dynamic_cast<const AssignExpr*>(&expr)) {
    compileExpr(*assign->value);
    emit(OpCode::StoreGlobal, addNameConstant(assign->name.lexeme));
    return;
  }
  if (const auto* grouping = dynamic_cast<const GroupingExpr*>(&expr)) {
    compileExpr(*grouping->expression);
    return;
  }
  if (const auto* unary = dynamic_cast<const UnaryExpr*>(&expr)) {
    compileExpr(*unary->right);
    switch (unary->op.type) {
      case TokenType::Minus: emit(OpCode::Negate); return;
      case TokenType::Bang:  emit(OpCode::Not);    return;
      default: throw std::runtime_error("Compiler error: unsupported unary operator.");
    }
  }
  if (const auto* binary = dynamic_cast<const BinaryExpr*>(&expr)) {
    compileExpr(*binary->left);
    compileExpr(*binary->right);
    switch (binary->op.type) {
      case TokenType::Plus:         emit(OpCode::Add);          return;
      case TokenType::Minus:        emit(OpCode::Subtract);     return;
      case TokenType::Star:         emit(OpCode::Multiply);     return;
      case TokenType::Slash:        emit(OpCode::Divide);       return;
      case TokenType::EqualEqual:   emit(OpCode::Equal);        return;
      case TokenType::BangEqual:    emit(OpCode::NotEqual);     return;
      case TokenType::Greater:      emit(OpCode::Greater);      return;
      case TokenType::GreaterEqual: emit(OpCode::GreaterEqual); return;
      case TokenType::Less:         emit(OpCode::Less);         return;
      case TokenType::LessEqual:    emit(OpCode::LessEqual);    return;
      default: throw std::runtime_error("Compiler error: unsupported binary operator.");
    }
  }
  if (dynamic_cast<const InputExpr*>(&expr)) {
    emit(OpCode::ReadInput);
    return;
  }
  throw std::runtime_error("Compiler error: unsupported expression node.");
}

int Compiler::addNameConstant(const std::string& name) {
  return chunk_.addConstant(name);
}

}  // namespace cvm
