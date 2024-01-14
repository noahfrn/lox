#include "AstInterpreter.h"
#include "Ast.h"
#include "Environment.h"
#include "Errors.h"
#include "Token.h"

#include <fmt/core.h>
#include <memory>
#include <string>
#include <variant>
#include <vector>

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

ObjectT AstInterpreter::Evaluate(const Expr &expr) { return std::visit(*this, expr); }

void AstInterpreter::Execute(const Stmt &stmt) { std::visit(*this, stmt); }

void AstInterpreter::ExecuteBlock(const std::vector<Stmt> &stmts, Environment environment)
{
  auto previous = environment_;
  try {
    environment_ = std::make_shared<Environment>(std::move(environment));
    for (const auto &stmt : stmts) { Execute(stmt); }
  } catch (...) {
    environment_ = std::move(previous);
    throw;
  }
  environment_ = std::move(previous);
}

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

auto AstInterpreter::operator()(const expr::Variable &variable) -> ObjectT { return environment_->Get(variable.name); }

auto AstInterpreter::operator()(const expr::Assign &assign) -> ObjectT
{
  auto value = Evaluate(*assign.value);
  environment_->Assign(assign.name, value);
  return value;
}


auto AstInterpreter::operator()(const stmt::Print &print) -> void
{
  auto value = Evaluate(*print.expression);
  fmt::print("{}\n", value);
}

auto AstInterpreter::operator()(const stmt::Expression &expression) -> void { Evaluate(*expression.expression); }

auto AstInterpreter::operator()(const stmt::Var &var) -> void
{
  if (var.initializer) {
    auto value = Evaluate(*var.initializer);
    environment_->Define(var.name.Lexeme(), value);
  } else {
    environment_->Define(var.name.Lexeme(), Nil{});
  }
}

auto AstInterpreter::operator()([[maybe_unused]] const stmt::Empty &empty) -> void {}

auto AstInterpreter::operator()(const stmt::Block &block) -> void
{
  ExecuteBlock(block.statements, Environment{ environment_ });
}

void AstInterpreter::Interpret(const std::vector<Stmt> &stmts)
{
  try {
    for (const auto &stmt : stmts) { Execute(stmt); }
  } catch (const RuntimeError &error) {
    error_reporter_->ReportRuntime(error.GetToken().Line(), error.what());
  }
}
