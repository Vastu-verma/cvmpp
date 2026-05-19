#include "cvm/Ast.h"

#include <sstream>

namespace cvm {

namespace {

std::string parenthesize(const std::string& name,
                         const std::vector<std::string>& parts) {
  std::ostringstream out;
  out << "(" << name;
  for (const auto& part : parts) out << " " << part;
  out << ")";
  return out.str();
}

}  // namespace

LiteralExpr::LiteralExpr(std::string value) : value(std::move(value)) {}
VariableExpr::VariableExpr(Token name) : name(std::move(name)) {}
AssignExpr::AssignExpr(Token name, ExprPtr value)
    : name(std::move(name)), value(std::move(value)) {}
UnaryExpr::UnaryExpr(Token op, ExprPtr right)
    : op(std::move(op)), right(std::move(right)) {}
BinaryExpr::BinaryExpr(ExprPtr left, Token op, ExprPtr right)
    : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}
GroupingExpr::GroupingExpr(ExprPtr expression)
    : expression(std::move(expression)) {}
InputExpr::InputExpr(Token keyword) : keyword(std::move(keyword)) {}
ExprStmt::ExprStmt(ExprPtr expression) : expression(std::move(expression)) {}
PrintStmt::PrintStmt(ExprPtr expression) : expression(std::move(expression)) {}
LetStmt::LetStmt(Token name, ExprPtr initializer)
    : name(std::move(name)), initializer(std::move(initializer)) {}
BlockStmt::BlockStmt(std::vector<StmtPtr> statements)
    : statements(std::move(statements)) {}
IfStmt::IfStmt(ExprPtr condition, StmtPtr thenBranch, StmtPtr elseBranch)
    : condition(std::move(condition)),
      thenBranch(std::move(thenBranch)),
      elseBranch(std::move(elseBranch)) {}
WhileStmt::WhileStmt(ExprPtr condition, StmtPtr body)
    : condition(std::move(condition)), body(std::move(body)) {}

std::string LiteralExpr::debugString() const { return value; }
std::string VariableExpr::debugString() const { return name.lexeme; }
std::string InputExpr::debugString() const { return "(input)"; }

std::string AssignExpr::debugString() const {
  return parenthesize("assign " + name.lexeme, {value->debugString()});
}
std::string UnaryExpr::debugString() const {
  return parenthesize(op.lexeme, {right->debugString()});
}
std::string BinaryExpr::debugString() const {
  return parenthesize(op.lexeme, {left->debugString(), right->debugString()});
}
std::string GroupingExpr::debugString() const {
  return parenthesize("group", {expression->debugString()});
}

std::string indent(int spaces) { return std::string(spaces, ' '); }

std::string ExprStmt::debugString(int indentLevel) const {
  return indent(indentLevel) + "ExprStmt " + expression->debugString();
}
std::string PrintStmt::debugString(int indentLevel) const {
  return indent(indentLevel) + "PrintStmt " + expression->debugString();
}
std::string LetStmt::debugString(int indentLevel) const {
  return indent(indentLevel) + "LetStmt " + name.lexeme + " = " +
         initializer->debugString();
}
std::string BlockStmt::debugString(int indentLevel) const {
  std::ostringstream out;
  out << indent(indentLevel) << "BlockStmt {\n";
  for (const auto& stmt : statements)
    out << stmt->debugString(indentLevel + 2) << "\n";
  out << indent(indentLevel) << "}";
  return out.str();
}
std::string IfStmt::debugString(int indentLevel) const {
  std::ostringstream out;
  out << indent(indentLevel) << "IfStmt condition=" << condition->debugString() << "\n";
  out << thenBranch->debugString(indentLevel + 2);
  if (elseBranch) {
    out << "\n" << indent(indentLevel) << "Else\n";
    out << elseBranch->debugString(indentLevel + 2);
  }
  return out.str();
}
std::string WhileStmt::debugString(int indentLevel) const {
  std::ostringstream out;
  out << indent(indentLevel) << "WhileStmt condition=" << condition->debugString() << "\n";
  out << body->debugString(indentLevel + 2);
  return out.str();
}

std::string debugProgram(const std::vector<StmtPtr>& statements) {
  std::ostringstream out;
  for (std::size_t i = 0; i < statements.size(); ++i) {
    out << statements[i]->debugString();
    if (i + 1 < statements.size()) out << "\n";
  }
  return out.str();
}

}  // namespace cvm
