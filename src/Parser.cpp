#include "Parser.h"
#include "Ast.h"
#include "Errors.h"
#include "Token.h"

#include <fmt/core.h>
#include <stdexcept>
#include <string_view>
#include <variant>
#include <vector>

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

ParseError Parser::Error(const Token &token, std::string_view message)
{
  if (token.Type() == TokenType::EOF_) {
    error_reporter_->Report(token.Line(), " at end", message);
  } else {
    error_reporter_->Report(token.Line(), fmt::format(" at '{}'", token.Lexeme()), message);
  }
  return { "Parse error." };
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

Stmt Parser::ParseDeclaration()
{
  try {
    if (Match(TokenType::VAR)) { return ParseVarDeclaration(); }

    return ParseStatement();
  } catch (const std::invalid_argument &) {
    Synchronize();
    return stmt::Empty{};
  }
}

Stmt Parser::ParseVarDeclaration()
{
  auto name = Consume(TokenType::IDENTIFIER, "Expect variable name.");

  ExprPtr initializer{};
  if (Match(TokenType::EQUAL)) { initializer = ParseExpression(); }

  Consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
  return stmt::Var{ name, initializer };
}

Stmt Parser::ParseStatement()
{
  if (Match(TokenType::PRINT)) { return ParsePrintStatement(); }
  if (Match(TokenType::LEFT_BRACE)) { return stmt::Block{ ParseBlock() }; }

  return ParseExpressionStatement();
}

Stmt Parser::ParsePrintStatement()
{
  auto value = ParseExpression();
  Consume(TokenType::SEMICOLON, "Expect ';' after value.");
  return stmt::Print(value);
}

std::vector<Stmt> Parser::ParseBlock()
{
  std::vector<Stmt> statements{};

  while (!Check(TokenType::RIGHT_BRACE) && !IsAtEnd()) { statements.push_back(ParseDeclaration()); }

  Consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
  return statements;
}

Stmt Parser::ParseExpressionStatement()
{
  auto expr = ParseExpression();
  Consume(TokenType::SEMICOLON, "Expect ';' after expression.");
  return stmt::Expression(expr);
}

ExprPtr Parser::ParseExpression() { return ParseAssign(); }

ExprPtr Parser::ParseAssign()
{
  auto expr = ParseEquality();

  if (Match(TokenType::EQUAL)) {
    auto equals = Previous();
    auto value = ParseAssign();

    if (std::holds_alternative<expr::Variable>(*expr)) {
      auto name = std::get<expr::Variable>(*expr).name;
      return MakeExpr<expr::Assign>(name, value);
    }

    Error(equals, "Invalid assignment target.");
  }

  return expr;
}

ExprPtr Parser::ParseEquality()
{
  auto expr = ParseComparison();

  while (Match(TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL)) {
    auto op = Previous();
    auto right = ParseComparison();
    expr = MakeExpr<expr::Binary>(expr, op, right);
  }

  return expr;
}

ExprPtr Parser::ParseComparison()
{
  auto expr = ParseTerm();

  while (Match(TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL)) {
    auto op = Previous();
    auto right = ParseTerm();
    expr = MakeExpr<expr::Binary>(expr, op, right);
  }

  return expr;
}

ExprPtr Parser::ParseTerm()
{
  auto expr = ParseFactor();

  while (Match(TokenType::MINUS, TokenType::PLUS)) {
    auto op = Previous();
    auto right = ParseFactor();
    expr = MakeExpr<expr::Binary>(expr, op, right);
  }

  return expr;
}

ExprPtr Parser::ParseFactor()
{
  auto expr = ParseUnary();

  while (Match(TokenType::SLASH, TokenType::STAR)) {
    auto op = Previous();
    auto right = ParseUnary();
    expr = MakeExpr<expr::Binary>(expr, op, right);
  }

  return expr;
}

ExprPtr Parser::ParseUnary()
{
  if (Match(TokenType::BANG, TokenType::MINUS)) {
    auto op = Previous();
    auto right = ParseUnary();
    return MakeExpr<expr::Unary>(op, right);
  }

  return ParsePrimary();
}

ExprPtr Parser::ParsePrimary()
{
  if (Match(TokenType::FALSE)) { return MakeExpr<expr::Literal>(false); }
  if (Match(TokenType::TRUE)) { return MakeExpr<expr::Literal>(true); }
  if (Match(TokenType::NIL)) { return MakeExpr<expr::Literal>(Nil{}); }

  if (Match(TokenType::NUMBER, TokenType::STRING)) { return MakeExpr<expr::Literal>(Previous().Literal()); }

  if (Match(TokenType::IDENTIFIER)) { return MakeExpr<expr::Variable>(Previous()); }

  if (Match(TokenType::LEFT_PAREN)) {
    auto expr = ParseExpression();
    Consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
    return MakeExpr<expr::Grouping>(expr);
  }

  throw Error(Peek(), "Expect expression.");
}

std::vector<Stmt> Parser::Parse()
{
  std::vector<Stmt> statements{};
  while (!IsAtEnd()) { statements.push_back(ParseDeclaration()); }
  return statements;
}
