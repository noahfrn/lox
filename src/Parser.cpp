#include "Parser.h"
#include "Ast.h"
#include "Token.h"

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
  throw std::runtime_error(fmt::format("[line {}] Error: {}", Peek().Line(), message));
}

ExprPtr Parser::Expression() { return Equality(); }

ExprPtr Parser::Equality()
{
  auto expr = Comparison();

  while (Match(TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL)) {
    auto op = Previous();
    auto right = Comparison();
    expr = MakeExpr<Binary>(expr, op, right);
  }

  return expr;
}

ExprPtr Parser::Comparison()
{
  auto expr = Term();

  while (Match(TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL)) {
    auto op = Previous();
    auto right = Term();
    expr = MakeExpr<Binary>(expr, op, right);
  }

  return expr;
}

ExprPtr Parser::Term()
{
  auto expr = Factor();

  while (Match(TokenType::MINUS, TokenType::PLUS)) {
    auto op = Previous();
    auto right = Factor();
    expr = MakeExpr<Binary>(expr, op, right);
  }

  return expr;
}

ExprPtr Parser::Factor()
{
  auto expr = Unary();

  while (Match(TokenType::SLASH, TokenType::STAR)) {
    auto op = Previous();
    auto right = Unary();
    expr = MakeExpr<Binary>(expr, op, right);
  }

  return expr;
}

ExprPtr Parser::Unary()
{
  if (Match(TokenType::BANG, TokenType::MINUS)) {
    auto op = Previous();
    auto right = Unary();
    return MakeExpr<Unary>(op, right);
  }

  return Primary();
}

ExprPtr Parser::Primary()
{
  if (Match(TokenType::FALSE)) { return MakeExpr<Literal>(false); }
  if (Match(TokenType::TRUE)) { return MakeExpr<Literal>(true); }
  if (Match(TokenType::NIL)) { return MakeExpr<Literal>(nullptr); }

  if (Match(TokenType::NUMBER, TokenType::STRING)) { return MakeExpr<Literal>(Previous().Literal()); }

  if (Match(TokenType::LEFT_PAREN)) {
    auto expr = Expression();
    Consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
    return MakeExpr<Grouping>(expr);
  }

  throw std::runtime_error("Expected expression.");
}
