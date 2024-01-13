#ifndef LOX_PARSER_H
#define LOX_PARSER_H

#include <string_view>
#include <utility>
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

  ExprPtr Parse();

private:
  std::vector<Token> tokens_;
  ErrorReporterPtr error_reporter_;
  int current_{ 0 };

  [[nodiscard]] ExprPtr ParseExpression();
  [[nodiscard]] ExprPtr ParseEquality();
  [[nodiscard]] ExprPtr ParseComparison();
  [[nodiscard]] ExprPtr ParseTerm();
  [[nodiscard]] ExprPtr ParseFactor();
  [[nodiscard]] ExprPtr ParseUnary();
  [[nodiscard]] ExprPtr ParsePrimary();

  template<typename... TokenTypes> bool Match(TokenTypes... types) { return (... || Match(types)); };
  [[nodiscard]] bool Match(TokenType type);
  [[nodiscard]] bool Check(TokenType type) const;
  [[nodiscard]] bool IsAtEnd() const;
  [[nodiscard]] Token Peek() const;
  [[nodiscard]] Token Previous() const;
  Token Advance();
  Token Consume(TokenType, std::string_view message);

  [[nodiscard]] std::exception Error(const Token& token, std::string_view message);

  void Synchronize();
};


#endif// LOX_PARSER_H
