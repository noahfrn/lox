#include "Parser.h"
#include "Ast.h"
#include "Errors.h"
#include "Token.h"

#include <fmt/core.h>
#include <memory>
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

ExprPtr Parser::FinishCall(ExprPtr callee)
{
  std::vector<ExprPtr> arguments{};

  if (!Check(TokenType::RIGHT_PAREN)) {
    do {
      if (arguments.size() >= 255) {
        Error(Peek(), "Can't have more than 255 arguments.");
      }
      arguments.push_back(ParseExpression());
    } while (Match(TokenType::COMMA));
  }

  auto paren = Consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");

  return MakeExpr<expr::Call>(callee, paren, arguments);
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
  } catch (const ParseError &) {
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
  if (Match(TokenType::FOR)) { return ParseFor(); }
  if (Match(TokenType::IF)) { return ParseIf(); }
  if (Match(TokenType::PRINT)) { return ParsePrint(); }
  if (Match(TokenType::WHILE)) { return ParseWhile(); }
  if (Match(TokenType::LEFT_BRACE)) { return stmt::Block{ ParseBlock() }; }

  return ParseExpressionStatement();
}

Stmt Parser::ParseFor()
{
  Consume(TokenType::LEFT_PAREN, "Expect '(' after 'for'.");

  Stmt initializer;
  if (Match(TokenType::SEMICOLON)) {
    initializer = stmt::Empty{};
  } else if (Match(TokenType::VAR)) {
    initializer = ParseVarDeclaration();
  } else {
    initializer = ParseExpressionStatement();
  }

  ExprPtr condition{};
  if (!Check(TokenType::SEMICOLON)) { condition = ParseExpression(); }
  Consume(TokenType::SEMICOLON, "Expect ';' after loop condition.");

  ExprPtr increment{};
  if (!Check(TokenType::RIGHT_PAREN)) { increment = ParseExpression(); }
  Consume(TokenType::RIGHT_PAREN, "Expect ')' after for clauses.");

  auto body = ParseStatement();

  if (increment) { body = stmt::Block{ { body, stmt::Expression{ increment } } }; }
  if (!condition) { condition = MakeExpr<expr::Literal>(true); }
  body = stmt::While{ condition, std::make_shared<Stmt>(body) };

  if (!std::holds_alternative<stmt::Empty>(initializer)) { body = stmt::Block{ { initializer, body } }; }

  return body;
}

Stmt Parser::ParseIf()
{
  Consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
  auto condition = ParseExpression();
  Consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition.");

  auto then_branch = std::make_shared<Stmt>(ParseStatement());
  return Match(TokenType::ELSE) ? stmt::If(condition, then_branch, std::make_shared<Stmt>(stmt::Empty{}))
                                : stmt::If(condition, then_branch, std::make_shared<Stmt>(ParseStatement()));
}

Stmt Parser::ParsePrint()
{
  auto value = ParseExpression();
  Consume(TokenType::SEMICOLON, "Expect ';' after value.");
  return stmt::Print(value);
}

Stmt Parser::ParseWhile()
{
  Consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
  auto condition = ParseExpression();
  Consume(TokenType::RIGHT_PAREN, "Expect ')' after condition.");
  auto body = ParseStatement();

  return stmt::While(condition, std::make_shared<Stmt>(body));
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
  auto expr = ParseOr();

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

ExprPtr Parser::ParseOr()
{
  auto expr = ParseAnd();

  while (Match(TokenType::OR)) {
    auto op = Previous();
    auto right = ParseAnd();
    expr = MakeExpr<expr::Logical>(expr, op, right);
  }

  return expr;
}

ExprPtr Parser::ParseAnd()
{
  auto expr = ParseEquality();

  while (Match(TokenType::AND)) {
    auto op = Previous();
    auto right = ParseEquality();
    expr = MakeExpr<expr::Logical>(expr, op, right);
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

  return ParseCall();
}

ExprPtr Parser::ParseCall()
{
  auto expr = ParsePrimary();

  while (true) {
    if (Match(TokenType::LEFT_PAREN)) {
      expr = FinishCall(expr);
    } else {
      break;
    }
  }

  return expr;
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
