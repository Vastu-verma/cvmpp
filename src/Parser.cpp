#include "cvm/Parser.h"

#include <initializer_list>
#include <iostream>
#include <stdexcept>

namespace cvm {

Parser::Parser(std::vector<Token> tokens) : tokens_(std::move(tokens)) {}

std::vector<StmtPtr> Parser::parse() {
  std::vector<StmtPtr> statements;
  while (!isAtEnd()) {
    auto stmt = declaration();
    if (stmt) statements.push_back(std::move(stmt));
  }
  return statements;
}

StmtPtr Parser::declaration() {
  try {
    if (match({TokenType::Let})) return letDeclaration();
    return statement();
  } catch (const ParseError&) {
    synchronize();
    return nullptr;
  }
}

StmtPtr Parser::letDeclaration() {
  Token name = consume(TokenType::Identifier, "Expected variable name.");
  consume(TokenType::Equal, "Expected '=' after variable name.");
  ExprPtr initializer = expression();
  consume(TokenType::Semicolon, "Expected ';' after variable declaration.");
  return std::make_unique<LetStmt>(name, std::move(initializer));
}

StmtPtr Parser::statement() {
  if (match({TokenType::Print})) return printStatement();
  if (match({TokenType::If})) return ifStatement();
  if (match({TokenType::While})) return whileStatement();
  if (match({TokenType::LeftBrace})) return blockStatement();
  return expressionStatement();
}

StmtPtr Parser::printStatement() {
  ExprPtr value = expression();
  consume(TokenType::Semicolon, "Expected ';' after value.");
  return std::make_unique<PrintStmt>(std::move(value));
}

StmtPtr Parser::expressionStatement() {
  ExprPtr expr = expression();
  consume(TokenType::Semicolon, "Expected ';' after expression.");
  return std::make_unique<ExprStmt>(std::move(expr));
}

StmtPtr Parser::blockStatement() {
  std::vector<StmtPtr> statements;
  while (!check(TokenType::RightBrace) && !isAtEnd()) {
    auto stmt = declaration();
    if (stmt) statements.push_back(std::move(stmt));
  }
  consume(TokenType::RightBrace, "Expected '}' after block.");
  return std::make_unique<BlockStmt>(std::move(statements));
}

StmtPtr Parser::ifStatement() {
  consume(TokenType::LeftParen, "Expected '(' after 'if'.");
  ExprPtr condition = expression();
  consume(TokenType::RightParen, "Expected ')' after if condition.");
  StmtPtr thenBranch = statement();
  StmtPtr elseBranch = nullptr;
  if (match({TokenType::Else})) elseBranch = statement();
  return std::make_unique<IfStmt>(std::move(condition), std::move(thenBranch),
                                  std::move(elseBranch));
}

StmtPtr Parser::whileStatement() {
  consume(TokenType::LeftParen, "Expected '(' after 'while'.");
  ExprPtr condition = expression();
  consume(TokenType::RightParen, "Expected ')' after while condition.");
  StmtPtr body = statement();
  return std::make_unique<WhileStmt>(std::move(condition), std::move(body));
}

ExprPtr Parser::expression() { return assignment(); }

ExprPtr Parser::assignment() {
  ExprPtr expr = equality();
  if (match({TokenType::Equal})) {
    Token equals = previous();
    ExprPtr value = assignment();
    if (auto* variable = dynamic_cast<VariableExpr*>(expr.get())) {
      return std::make_unique<AssignExpr>(variable->name, std::move(value));
    }
    throw error(equals, "Invalid assignment target.");
  }
  return expr;
}

ExprPtr Parser::equality() {
  ExprPtr expr = comparison();
  while (match({TokenType::BangEqual, TokenType::EqualEqual})) {
    Token op = previous();
    ExprPtr right = comparison();
    expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
  }
  return expr;
}

ExprPtr Parser::comparison() {
  ExprPtr expr = term();
  while (match({TokenType::Greater, TokenType::GreaterEqual, TokenType::Less,
                TokenType::LessEqual})) {
    Token op = previous();
    ExprPtr right = term();
    expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
  }
  return expr;
}

ExprPtr Parser::term() {
  ExprPtr expr = factor();
  while (match({TokenType::Plus, TokenType::Minus})) {
    Token op = previous();
    ExprPtr right = factor();
    expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
  }
  return expr;
}

ExprPtr Parser::factor() {
  ExprPtr expr = unary();
  while (match({TokenType::Star, TokenType::Slash})) {
    Token op = previous();
    ExprPtr right = unary();
    expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
  }
  return expr;
}

ExprPtr Parser::unary() {
  if (match({TokenType::Bang, TokenType::Minus})) {
    Token op = previous();
    ExprPtr right = unary();
    return std::make_unique<UnaryExpr>(op, std::move(right));
  }
  return primary();
}

ExprPtr Parser::primary() {
  if (match({TokenType::False})) return std::make_unique<LiteralExpr>("false");
  if (match({TokenType::True})) return std::make_unique<LiteralExpr>("true");
  if (match({TokenType::Number})) return std::make_unique<LiteralExpr>(previous().lexeme);
  if (match({TokenType::Identifier})) return std::make_unique<VariableExpr>(previous());
  if (match({TokenType::Input})) return std::make_unique<InputExpr>(previous());
  if (match({TokenType::LeftParen})) {
    ExprPtr expr = expression();
    consume(TokenType::RightParen, "Expected ')' after expression.");
    return std::make_unique<GroupingExpr>(std::move(expr));
  }
  throw error(peek(), "Expected expression.");
}

bool Parser::isAtEnd() const { return peek().type == TokenType::EndOfFile; }
const Token& Parser::peek() const { return tokens_[current_]; }
const Token& Parser::previous() const { return tokens_[current_ - 1]; }
const Token& Parser::advance() {
  if (!isAtEnd()) ++current_;
  return previous();
}
bool Parser::check(TokenType type) const {
  if (isAtEnd()) return false;
  return peek().type == type;
}
bool Parser::match(std::initializer_list<TokenType> types) {
  for (TokenType type : types) {
    if (check(type)) { advance(); return true; }
  }
  return false;
}
const Token& Parser::consume(TokenType type, const std::string& message) {
  if (check(type)) return advance();
  throw error(peek(), message);
}
Parser::ParseError Parser::error(const Token& token, const std::string& message) const {
  std::cerr << "[Parser error] line " << token.line << ": " << message;
  if (!token.lexeme.empty()) std::cerr << " near '" << token.lexeme << "'";
  std::cerr << "\n";
  return ParseError{message};
}
void Parser::synchronize() {
  advance();
  while (!isAtEnd()) {
    if (previous().type == TokenType::Semicolon) return;
    switch (peek().type) {
      case TokenType::Let:
      case TokenType::Print:
      case TokenType::If:
      case TokenType::While:
        return;
      default: break;
    }
    advance();
  }
}

}  // namespace cvm
