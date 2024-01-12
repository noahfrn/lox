#ifndef LOX_PARSER_H
#define LOX_PARSER_H

#include <string_view>
#include <vector>

#include "Ast.h"
#include "ErrorReporter.h"
#include "Token.h"

class Parser
{
public:
  Parser() = default;
  Parser(std::vector<Token> tokens, ErrorReporterPtr reporter)
    : tokens_{ std::move(tokens) }, error_reporter_{ reporter }
  {}

private:
  std::vector<Token> tokens_;
  ErrorReporterPtr error_reporter_;
  int current_{ 0 };

  [[nodiscard]] ExprPtr Expression();
  [[nodiscard]] ExprPtr Equality();
  [[nodiscard]] ExprPtr Comparison();
  [[nodiscard]] ExprPtr Term();
  [[nodiscard]] ExprPtr Factor();
  [[nodiscard]] ExprPtr Unary();
  [[nodiscard]] ExprPtr Primary();

  template<typename... TokenTypes> bool Match(TokenTypes... types) { return (... && Match(types)); };
  [[nodiscard]] bool Match(TokenType type);
  [[nodiscard]] bool Check(TokenType type) const;
  [[nodiscard]] bool IsAtEnd() const;
  [[nodiscard]] Token Peek() const;
  [[nodiscard]] Token Previous() const;
  Token Advance();
  Token Consume(TokenType, std::string_view message);
};


#endif// LOX_PARSER_H
