#include "Scanner.h"
#include "Token.h"
#include <fmt/core.h>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>


bool Scanner::IsAtEnd() const { return current_ >= source_.size(); }

bool Scanner::IsDigit(char c) { return c >= '0' && c <= '9'; }

bool Scanner::IsAlpha(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'; }

bool Scanner::IsAlphaNumeric(char c) { return IsAlpha(c) || IsDigit(c); }

char Scanner::Advance() { return source_.at(current_++); }

bool Scanner::Match(char expected)
{
  if (IsAtEnd()) { return false; }
  if (source_.at(current_) != expected) { return false; }

  ++current_;
  return true;
}

char Scanner::Peek() const
{
  if (IsAtEnd()) { return '\0'; }
  return source_.at(current_);
}

char Scanner::PeekNext() const
{
  if (current_ + 1 >= source_.size()) { return '\0'; }
  return source_.at(current_ + 1);
}

void Scanner::String()
{
  while (Peek() != '"' && !IsAtEnd()) {
    if (Peek() == '\n') { ++line_; }
    Advance();
  }

  if (IsAtEnd()) {
    error_reporter_->Report(line_, "", "Unterminated string.");
    return;
  }

  Advance();

  auto value = source_.substr(start_ + 1, current_ - start_ - 2);
  AddToken(TokenType::STRING, value);
}

void Scanner::Number()
{
  while (IsDigit(Peek())) { Advance(); }

  // Look for a fractional part.
  if (Peek() == '.' && IsDigit(PeekNext())) {
    // Consume the "."
    Advance();

    while (IsDigit(Peek())) { Advance(); }
  }

  AddToken(TokenType::NUMBER, std::stod(source_.substr(start_, current_ - start_)));
}

void Scanner::BlockComment()
{
  while (Peek() != '*' && PeekNext() != '/' && !IsAtEnd()) {
    if (Peek() == '\n') { ++line_; }
    Advance();
  }
  if (IsAtEnd()) {
    error_reporter_->Report(line_, "", "Unterminated comment.");
    return;
  }
  Advance();
  Advance();
}

void Scanner::Identifier()
{
  while (IsAlphaNumeric(Peek())) { Advance(); }

  static std::unordered_map<std::string, TokenType> keywords_{
    { "and", TokenType::AND },
    { "class", TokenType::CLASS },
    { "else", TokenType::ELSE },
    { "false", TokenType::FALSE },
    { "for", TokenType::FOR },
    { "fun", TokenType::FUN },
    { "if", TokenType::IF },
    { "nil", TokenType::NIL },
    { "or", TokenType::OR },
    { "print", TokenType::PRINT },
    { "return", TokenType::RETURN },
    { "super", TokenType::SUPER },
    { "this", TokenType::THIS },
    { "true", TokenType::TRUE },
    { "var", TokenType::VAR },
    { "while", TokenType::WHILE },
  };


  // See if the identifier is a reserved word.
  auto text = source_.substr(start_, current_ - start_);
  auto type = keywords_.find(text);
  if (type == keywords_.end()) {
    AddToken(TokenType::IDENTIFIER);
  } else {
    AddToken(type->second);
  }
}

void Scanner::AddToken(TokenType type) { AddToken(type, std::monostate{}); }

void Scanner::AddToken(TokenType type, const LiteralT &literal)
{
  const std::string text = source_.substr(start_, current_ - start_);
  tokens_.emplace_back(type, text, literal, line_);
}

void Scanner::ScanToken()
{
  auto c = Advance();
  switch (c) {
  case '(':
    AddToken(TokenType::LEFT_PAREN);
    break;
  case ')':
    AddToken(TokenType::RIGHT_PAREN);
    break;
  case '{':
    AddToken(TokenType::LEFT_BRACE);
    break;
  case '}':
    AddToken(TokenType::RIGHT_BRACE);
    break;
  case ',':
    AddToken(TokenType::COMMA);
    break;
  case '.':
    AddToken(TokenType::DOT);
    break;
  case '-':
    AddToken(TokenType::MINUS);
    break;
  case '+':
    AddToken(TokenType::PLUS);
    break;
  case ';':
    AddToken(TokenType::SEMICOLON);
    break;
  case '*':
    AddToken(TokenType::STAR);
    break;
  case '!':
    AddToken(Match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
    break;
  case '=':
    AddToken(Match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
    break;
  case '<':
    AddToken(Match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
    break;
  case '>':
    AddToken(Match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
    break;
  case '/':
    if (Match('/')) {
      // A comment goes until the end of the line.
      while (Peek() != '\n' && !IsAtEnd()) { Advance(); }
    } else if (Match('*')) {
      BlockComment();
    } else {
      AddToken(TokenType::SLASH);
    }
    break;
  case ' ':
  case '\r':
  case '\t':
    break;
  case '\n':
    line_++;
    break;
  case '"':
    String();
    break;
  default:
    if (IsDigit(c)) {
      Number();
    } else if (IsAlpha(c)) {
      Identifier();
    } else {
      error_reporter_->Report(line_, "", fmt::format("Unexpected character: {}", c));
    }
    break;
  }
}

std::vector<Token> Scanner::ScanTokens()
{
  while (!IsAtEnd()) {
    start_ = current_;
    ScanToken();
  }

  tokens_.emplace_back(TokenType::EOF_, "", std::monostate{}, line_);

  return tokens_;
}
