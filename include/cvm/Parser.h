#pragma once

#include <string>
#include <vector>

#include "cvm/Ast.h"
#include "cvm/Token.h"

namespace cvm {

class Parser {
 public:
  explicit Parser(std::vector<Token> tokens);
  std::vector<StmtPtr> parse();

 private:
  struct ParseError {
    std::string message;
  };

  StmtPtr declaration();
  StmtPtr letDeclaration();
  StmtPtr statement();
  StmtPtr printStatement();
  StmtPtr expressionStatement();
  StmtPtr blockStatement();
  StmtPtr ifStatement();
  StmtPtr whileStatement();

  ExprPtr expression();
  ExprPtr assignment();
  ExprPtr equality();
  ExprPtr comparison();
  ExprPtr term();
  ExprPtr factor();
  ExprPtr unary();
  ExprPtr primary();

  bool isAtEnd() const;
  const Token& peek() const;
  const Token& previous() const;
  const Token& advance();
  bool check(TokenType type) const;
  bool match(std::initializer_list<TokenType> types);
  const Token& consume(TokenType type, const std::string& message);
  ParseError error(const Token& token, const std::string& message) const;
  void synchronize();

  std::vector<Token> tokens_;
  std::size_t current_ = 0;
};

}  // namespace cvm
