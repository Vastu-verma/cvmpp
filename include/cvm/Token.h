#pragma once

#include <string>

namespace cvm {

enum class TokenType {
  LeftParen,
  RightParen,
  LeftBrace,
  RightBrace,
  Semicolon,
  Plus,
  Minus,
  Star,
  Slash,
  Bang,
  BangEqual,
  Equal,
  EqualEqual,
  Greater,
  GreaterEqual,
  Less,
  LessEqual,
  Identifier,
  Number,
  Let,
  Print,
  If,
  Else,
  While,
  True,
  False,
  Input,
  EndOfFile
};

struct Token {
  TokenType type;
  std::string lexeme;
  int line;
};

std::string tokenTypeToString(TokenType type);

}  // namespace cvm
