#include "cvm/Lexer.h"

#include <cctype>
#include <stdexcept>
#include <unordered_map>

namespace cvm {

namespace {

const std::unordered_map<std::string, TokenType> kKeywords = {
    {"let", TokenType::Let},     {"print", TokenType::Print},
    {"if", TokenType::If},       {"else", TokenType::Else},
    {"while", TokenType::While}, {"true", TokenType::True},
    {"false", TokenType::False}, {"input", TokenType::Input},
};

}  // namespace

std::string tokenTypeToString(TokenType type) {
  switch (type) {
    case TokenType::LeftParen: return "LeftParen";
    case TokenType::RightParen: return "RightParen";
    case TokenType::LeftBrace: return "LeftBrace";
    case TokenType::RightBrace: return "RightBrace";
    case TokenType::Semicolon: return "Semicolon";
    case TokenType::Plus: return "Plus";
    case TokenType::Minus: return "Minus";
    case TokenType::Star: return "Star";
    case TokenType::Slash: return "Slash";
    case TokenType::Bang: return "Bang";
    case TokenType::BangEqual: return "BangEqual";
    case TokenType::Equal: return "Equal";
    case TokenType::EqualEqual: return "EqualEqual";
    case TokenType::Greater: return "Greater";
    case TokenType::GreaterEqual: return "GreaterEqual";
    case TokenType::Less: return "Less";
    case TokenType::LessEqual: return "LessEqual";
    case TokenType::Identifier: return "Identifier";
    case TokenType::Number: return "Number";
    case TokenType::Let: return "Let";
    case TokenType::Print: return "Print";
    case TokenType::If: return "If";
    case TokenType::Else: return "Else";
    case TokenType::While: return "While";
    case TokenType::True: return "True";
    case TokenType::False: return "False";
    case TokenType::Input: return "Input";
    case TokenType::EndOfFile: return "EOF";
  }
  return "Unknown";
}

Lexer::Lexer(std::string source) : source_(std::move(source)) {}

std::vector<Token> Lexer::scanTokens() {
  while (!isAtEnd()) {
    start_ = current_;
    scanToken();
  }
  tokens_.push_back({TokenType::EndOfFile, "", line_});
  return tokens_;
}

bool Lexer::isAtEnd() const { return current_ >= source_.size(); }

char Lexer::advance() { return source_[current_++]; }

char Lexer::peek() const { return isAtEnd() ? '\0' : source_[current_]; }

char Lexer::peekNext() const {
  if (current_ + 1 >= source_.size()) return '\0';
  return source_[current_ + 1];
}

bool Lexer::match(char expected) {
  if (isAtEnd() || source_[current_] != expected) return false;
  ++current_;
  return true;
}

void Lexer::scanToken() {
  skipWhitespace();
  if (isAtEnd()) return;

  start_ = current_;
  char c = advance();
  switch (c) {
    case '(': addToken(TokenType::LeftParen); break;
    case ')': addToken(TokenType::RightParen); break;
    case '{': addToken(TokenType::LeftBrace); break;
    case '}': addToken(TokenType::RightBrace); break;
    case ';': addToken(TokenType::Semicolon); break;
    case '+': addToken(TokenType::Plus); break;
    case '-': addToken(TokenType::Minus); break;
    case '*': addToken(TokenType::Star); break;
    case '/':
      if (match('/')) {
        while (peek() != '\n' && !isAtEnd()) advance();
      } else {
        addToken(TokenType::Slash);
      }
      break;
    case '!': addToken(match('=') ? TokenType::BangEqual : TokenType::Bang); break;
    case '=': addToken(match('=') ? TokenType::EqualEqual : TokenType::Equal); break;
    case '>': addToken(match('=') ? TokenType::GreaterEqual : TokenType::Greater); break;
    case '<': addToken(match('=') ? TokenType::LessEqual : TokenType::Less); break;
    default:
      if (std::isdigit(static_cast<unsigned char>(c))) {
        number();
      } else if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
        identifier();
      } else {
        throw std::runtime_error("Lexer error at line " + std::to_string(line_) +
                                 ": unexpected character '" + c + "'");
      }
  }
}

void Lexer::addToken(TokenType type) {
  tokens_.push_back({type, source_.substr(start_, current_ - start_), line_});
}

void Lexer::identifier() {
  while (std::isalnum(static_cast<unsigned char>(peek())) || peek() == '_') {
    advance();
  }
  std::string text = source_.substr(start_, current_ - start_);
  auto it = kKeywords.find(text);
  if (it != kKeywords.end()) {
    tokens_.push_back({it->second, text, line_});
  } else {
    tokens_.push_back({TokenType::Identifier, text, line_});
  }
}

void Lexer::number() {
  while (std::isdigit(static_cast<unsigned char>(peek()))) advance();
  if (peek() == '.' && std::isdigit(static_cast<unsigned char>(peekNext()))) {
    advance();
    while (std::isdigit(static_cast<unsigned char>(peek()))) advance();
  }
  addToken(TokenType::Number);
}

void Lexer::skipWhitespace() {
  while (!isAtEnd()) {
    char c = peek();
    switch (c) {
      case ' ': case '\r': case '\t': advance(); break;
      case '\n': ++line_; advance(); break;
      default: return;
    }
  }
}

}  // namespace cvm
