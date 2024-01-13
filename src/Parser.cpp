#include "Parser.h"
#include "Ast.h"
#include "Token.h"
#include <exception>
#include <fmt/core.h>
#include <stdexcept>
#include <string_view>

Token Parser::Previous() const { return tokens_.at(current_ - 1); }

Token Parser::Peek() const { return tokens_.at(current_); }

Token Parser::Advance()
{
  if (!IsAtEnd()) { current_++; }
  return Previous();
}

bool Parser::IsAtEnd() const { return Peek().Type() == TokenType::EOF_; }

bool Parser::Check(TokenType type) const
{
  if (IsAtEnd()) { return false; }
  return Peek().Type() == type;
}

bool Parser::Match(TokenType type)
{
  if (Check(type)) {
    Advance();
    return true;
  }
  return false;
}

Token Parser::Consume(TokenType type, std::string_view message)
{
  if (Check(type)) { return Advance(); }
  throw Error(Peek(), message);
}

std::exception Parser::Error(Token token, std::string_view message)
{
  if (token.Type() == TokenType::EOF_) {
    error_reporter_->Report(token.Line(), " at end", message);
  } else {
    error_reporter_->Report(token.Line(), fmt::format(" at '{}'", token.Lexeme()), message);
  }
  return std::runtime_error("Parse error.");
}

void Parser::Synchronize()
{
  Advance();

  while (!IsAtEnd()) {
    if (Previous().Type() == TokenType::SEMICOLON) { return; }

    switch (Peek().Type()) {
    case TokenType::CLASS:
    case TokenType::FUN:
    case TokenType::VAR:
    case TokenType::FOR:
    case TokenType::IF:
    case TokenType::WHILE:
    case TokenType::PRINT:
    case TokenType::RETURN:
      return;
    default:
      break;
    }

    Advance();
  }
}

ExprPtr Parser::ParseExpression() { return ParseEquality(); }

ExprPtr Parser::ParseEquality()
{
  auto expr = ParseComparison();

  while (Match(TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL)) {
    auto op = Previous();
    auto right = ParseComparison();
    expr = MakeExpr<Binary>(expr, op, right);
  }

  return expr;
}

ExprPtr Parser::ParseComparison()
{
  auto expr = ParseTerm();

  while (Match(TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL)) {
    auto op = Previous();
    auto right = ParseTerm();
    expr = MakeExpr<Binary>(expr, op, right);
  }

  return expr;
}

ExprPtr Parser::ParseTerm()
{
  auto expr = ParseFactor();

  while (Match(TokenType::MINUS, TokenType::PLUS)) {
    auto op = Previous();
    auto right = ParseFactor();
    expr = MakeExpr<Binary>(expr, op, right);
  }

  return expr;
}

ExprPtr Parser::ParseFactor()
{
  auto expr = ParseUnary();

  while (Match(TokenType::SLASH, TokenType::STAR)) {
    auto op = Previous();
    auto right = ParseUnary();
    expr = MakeExpr<Binary>(expr, op, right);
  }

  return expr;
}

ExprPtr Parser::ParseUnary()
{
  if (Match(TokenType::BANG, TokenType::MINUS)) {
    auto op = Previous();
    auto right = ParseUnary();
    return MakeExpr<Unary>(op, right);
  }

  return ParsePrimary();
}

ExprPtr Parser::ParsePrimary()
{
  if (Match(TokenType::FALSE)) { return MakeExpr<Literal>(false); }
  if (Match(TokenType::TRUE)) { return MakeExpr<Literal>(true); }
  if (Match(TokenType::NIL)) { return MakeExpr<Literal>(Nil{}); }

  if (Match(TokenType::NUMBER, TokenType::STRING)) { return MakeExpr<Literal>(Previous().Literal()); }

  if (Match(TokenType::LEFT_PAREN)) {
    auto expr = ParseExpression();
    Consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
    return MakeExpr<Grouping>(expr);
  }

  throw Error(Peek(), "Expect expression.");
}

ExprPtr Parser::Parse()
{
  try {
    return ParseExpression();
  } catch (std::exception &e) {
    return nullptr;
  }
}
