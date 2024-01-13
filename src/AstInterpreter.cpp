#include "AstInterpreter.h"
#include "Ast.h"
#include "Errors.h"
#include "Token.h"

#include <fmt/core.h>
#include <string>
#include <variant>

bool AstInterpreter::IsTruthy(const ObjectT &object)
{
  if (std::holds_alternative<Nil>(object)) {
    return false;
  } else if (std::holds_alternative<bool>(object)) {
    return std::get<bool>(object);
  } else {
    return true;
  }
}

bool AstInterpreter::IsEqual(const ObjectT &left, const ObjectT &right)
{
  if (std::holds_alternative<Nil>(left) && std::holds_alternative<Nil>(right)) {
    return true;
  } else if (std::holds_alternative<Nil>(left)) {
    return false;
  }

  return left == right;
}

void AstInterpreter::CheckNumberOperand(const Token &op, const ObjectT &operand)
{
  if (std::holds_alternative<double>(operand)) { return; }
  throw RuntimeError(op, fmt::format("Operand must be a number: {}", op.Lexeme()));
}

void AstInterpreter::CheckNumberOperands(const Token &op, const ObjectT &left, const ObjectT &right)
{
  if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) { return; }
  throw RuntimeError(op, fmt::format("Operands must be numbers: {} {}", left, right));
}

auto AstInterpreter::Evaluate(const Expr &expr) -> ObjectT { return std::visit(*this, expr); }

auto AstInterpreter::Execute(const Stmt &stmt) -> void { std::visit(*this, stmt); }

auto AstInterpreter::operator()(const expr::Unary &unary) -> ObjectT
{
  auto right = Evaluate(*unary.right);
  switch (unary.op.Type()) {
  case TokenType::MINUS:
    CheckNumberOperand(unary.op, right);
    return -std::get<double>(right);
  case TokenType::BANG:
    return !IsTruthy(right);
  default:
    return Nil{};
  }
}

auto AstInterpreter::operator()(const expr::Binary &binary) -> ObjectT
{
  auto left = Evaluate(*binary.left);
  auto right = Evaluate(*binary.right);

  switch (binary.op.Type()) {
  case TokenType::MINUS:
    CheckNumberOperands(binary.op, left, right);
    return std::get<double>(left) - std::get<double>(right);
  case TokenType::SLASH:
    CheckNumberOperands(binary.op, left, right);
    return std::get<double>(left) / std::get<double>(right);
  case TokenType::STAR:
    CheckNumberOperands(binary.op, left, right);
    return std::get<double>(left) * std::get<double>(right);
  case TokenType::PLUS:
    if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
      return std::get<double>(left) + std::get<double>(right);
    } else if (std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right)) {
      return std::get<std::string>(left) + std::get<std::string>(right);
    }
    throw RuntimeError(binary.op, fmt::format("Operands must be two numbers or two strings: {} {}", left, right));
  case TokenType::GREATER:
    CheckNumberOperands(binary.op, left, right);
    return std::get<double>(left) > std::get<double>(right);
  case TokenType::GREATER_EQUAL:
    CheckNumberOperands(binary.op, left, right);
    return std::get<double>(left) >= std::get<double>(right);
  case TokenType::LESS:
    CheckNumberOperands(binary.op, left, right);
    return std::get<double>(left) < std::get<double>(right);
  case TokenType::LESS_EQUAL:
    CheckNumberOperands(binary.op, left, right);
    return std::get<double>(left) <= std::get<double>(right);
  case TokenType::BANG_EQUAL:
    return !IsEqual(left, right);
  case TokenType::EQUAL_EQUAL:
    return IsEqual(left, right);
  default:
    return Nil{};
  }
}

auto AstInterpreter::operator()(const expr::Literal &literal) -> ObjectT { return literal.value; }

auto AstInterpreter::operator()(const expr::Grouping &grouping) -> ObjectT { return Evaluate(*grouping.expression); }

ObjectT AstInterpreter::operator()(const expr::Variable &variable) { return environment_.Get(variable.name); }

auto AstInterpreter::operator()(const stmt::Print &print) -> ObjectT
{
  auto value = Evaluate(*print.expression);
  fmt::print("{}\n", value);
  return Nil{};
}

auto AstInterpreter::operator()(const stmt::Expression &expression) -> ObjectT
{
  Evaluate(*expression.expression);
  return Nil{};
}

auto AstInterpreter::operator()(const stmt::Var &var) -> ObjectT
{
  if (var.initializer) {
    auto value = Evaluate(*var.initializer);
    environment_.Define(var.name.Lexeme(), value);
  } else {
    environment_.Define(var.name.Lexeme(), Nil{});
  }

  return Nil{};
}

auto AstInterpreter::operator()(const stmt::Empty &empty) -> ObjectT { return Nil{}; }

void AstInterpreter::Interpret(const std::vector<Stmt> &stmts)
{
  try {
    for (const auto &stmt : stmts) { Execute(stmt); }
  } catch (const RuntimeError &error) {
    error_reporter_->ReportRuntime(error.GetToken().Line(), error.what());
  }
}
