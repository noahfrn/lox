#ifndef LOX_TOKEN_H
#define LOX_TOKEN_H

#include <any>
#include <cstdint>
#include <fmt/format.h>
#include <magic_enum/magic_enum.hpp>
#include <string>
#include <variant>

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

using LiteralT = std::variant<std::monostate, bool, double, std::string>;

template<> struct fmt::formatter<LiteralT>
{
  template<typename ParseContext> constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

  template<typename FormatContext> auto format(const LiteralT &literal, FormatContext &ctx)
  {
    return std::visit(
      [&ctx](auto &&arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::monostate>) {
          return fmt::format_to(ctx.out(), "nil");
        } else {
          return fmt::format_to(ctx.out(), "{}", arg);
        }
      },
      literal);
  }
};

class Token
{
public:
  Token(TokenType type, std::string lexeme, LiteralT literal, int line)
    : type_{ type }, lexeme_{ std::move(lexeme) }, literal_{ std::move(literal) }, line_{ line }
  {}

  [[nodiscard]] TokenType Type() const { return type_; }
  [[nodiscard]] const std::string &Lexeme() const { return lexeme_; }
  [[nodiscard]] const LiteralT &Literal() const { return literal_; }
  [[nodiscard]] int Line() const { return line_; }

private:
  TokenType type_;
  std::string lexeme_;
  LiteralT literal_;
  int line_;
};

template<> struct fmt::formatter<Token>
{
  template<typename ParseContext> constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

  template<typename FormatContext> auto format(const Token &token, FormatContext &ctx)
  {
    return fmt::format_to(ctx.out(), "{} {} {}", magic_enum::enum_name(token.Type()), token.Lexeme(), token.Literal());
  }
};


#endif// LOX_TOKEN_H
