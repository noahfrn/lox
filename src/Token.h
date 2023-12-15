#ifndef LOX_TOKEN_H
#define LOX_TOKEN_H

#include <any>
#include <cstdint>
#include <fmt/format.h>
#include <string>

enum class TokenType : std::uint8_t {
  LEFT_PAREN,
  RIGHT_PAREN,
  LEFT_BRACE,
  RIGHT_BRACE,
  COMMA,
  DOT,
  SLASH,
  STAR,
  MINUS,
  PLUS,
  SEMICOLON,

  BANG,
  BANG_EQUAL,
  EQUAL,
  EQUAL_EQUAL,
  GREATER,
  GREATER_EQUAL,
  LESS,
  LESS_EQUAL,

  IDENTIFIER,
  STRING,
  NUMBER,

  AND,
  CLASS,
  ELSE,
  FALSE,
  FUN,
  FOR,
  IF,
  NIL,
  OR,
  PRINT,
  RETURN,
  SUPER,
  THIS,
  TRUE,
  VAR,
  WHILE,

  EOF_
};


template<ObjectType>
class Token
{
public:
  Token(TokenType type, std::string lexeme, std::any literal, int line)
    : type_{ type }, lexeme_{ std::move(lexeme) }, literal_{ std::move(literal) }, line_{ line }
  {}
  TokenType Type() const { return type_; }
  const std::string &Lexeme() const { return lexeme_; }
  const any &Literal() const { return literal_; }
  int Line() const { return line_; }

private:
  TokenType type_;
  std::string lexeme_;
  std::unique_ptr<ObjectType> literal_;
  int line_;
};

template<> struct fmt::formatter<Token>
{
  template<typename ParseContext> constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

  template<typename FormatContext> auto format(const Token &token, FormatContext &ctx)
  {
    return fmt::format_to(ctx.out(), "{} {} {}", token.Type(), token.Lexeme(), token.Literal());
  }
};


#endif// LOX_TOKEN_H
