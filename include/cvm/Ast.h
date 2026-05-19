#pragma once

#include <memory>
#include <string>
#include <vector>

#include "cvm/Token.h"

namespace cvm {

struct Expr {
  virtual ~Expr() = default;
  virtual std::string debugString() const = 0;
};

using ExprPtr = std::unique_ptr<Expr>;

struct LiteralExpr : Expr {
  explicit LiteralExpr(std::string value);
  std::string value;
  std::string debugString() const override;
};

struct VariableExpr : Expr {
  explicit VariableExpr(Token name);
  Token name;
  std::string debugString() const override;
};

struct AssignExpr : Expr {
  AssignExpr(Token name, ExprPtr value);
  Token name;
  ExprPtr value;
  std::string debugString() const override;
};

struct UnaryExpr : Expr {
  UnaryExpr(Token op, ExprPtr right);
  Token op;
  ExprPtr right;
  std::string debugString() const override;
};

struct BinaryExpr : Expr {
  BinaryExpr(ExprPtr left, Token op, ExprPtr right);
  ExprPtr left;
  Token op;
  ExprPtr right;
  std::string debugString() const override;
};

struct GroupingExpr : Expr {
  explicit GroupingExpr(ExprPtr expression);
  ExprPtr expression;
  std::string debugString() const override;
};

struct InputExpr : Expr {
  explicit InputExpr(Token keyword);
  Token keyword;
  std::string debugString() const override;
};

struct Stmt {
  virtual ~Stmt() = default;
  virtual std::string debugString(int indent = 0) const = 0;
};

using StmtPtr = std::unique_ptr<Stmt>;

struct ExprStmt : Stmt {
  explicit ExprStmt(ExprPtr expression);
  ExprPtr expression;
  std::string debugString(int indent = 0) const override;
};

struct PrintStmt : Stmt {
  explicit PrintStmt(ExprPtr expression);
  ExprPtr expression;
  std::string debugString(int indent = 0) const override;
};

struct LetStmt : Stmt {
  LetStmt(Token name, ExprPtr initializer);
  Token name;
  ExprPtr initializer;
  std::string debugString(int indent = 0) const override;
};

struct BlockStmt : Stmt {
  explicit BlockStmt(std::vector<StmtPtr> statements);
  std::vector<StmtPtr> statements;
  std::string debugString(int indent = 0) const override;
};

struct IfStmt : Stmt {
  IfStmt(ExprPtr condition, StmtPtr thenBranch, StmtPtr elseBranch);
  ExprPtr condition;
  StmtPtr thenBranch;
  StmtPtr elseBranch;
  std::string debugString(int indent = 0) const override;
};

struct WhileStmt : Stmt {
  WhileStmt(ExprPtr condition, StmtPtr body);
  ExprPtr condition;
  StmtPtr body;
  std::string debugString(int indent = 0) const override;
};

std::string indent(int spaces);
std::string debugProgram(const std::vector<StmtPtr>& statements);

}  // namespace cvm
