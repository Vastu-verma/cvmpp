#pragma once

#include <string>
#include <vector>

#include "cvm/Token.h"

namespace cvm {

class Lexer {
 public:
  explicit Lexer(std::string source);
  std::vector<Token> scanTokens();

 private:
  bool isAtEnd() const;
  char advance();
  char peek() const;
  char peekNext() const;
  bool match(char expected);
  void scanToken();
  void addToken(TokenType type);
  void identifier();
  void number();
  void skipWhitespace();

  std::string source_;
  std::vector<Token> tokens_;
  std::size_t start_ = 0;
  std::size_t current_ = 0;
  int line_ = 1;
};

}  // namespace cvm
