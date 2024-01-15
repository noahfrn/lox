#ifndef LOX_ASTINTERPRETER_H
#define LOX_ASTINTERPRETER_H

#include "Ast.h"
#include "ClockCallable.h"
#include "Common.h"
#include "Environment.h"
#include "ErrorReporter.h"
#include "Token.h"

#include <chrono>
#include <memory>
#include <utility>
#include <vector>

class AstInterpreter
{
public:
  explicit AstInterpreter(ErrorReporterPtr error_reporter) : error_reporter_{ std::move(error_reporter) }
  {
    // Also implement arity
    globals_->Define("clock", std::make_unique<ClockCallable>());
  }
  void Interpret(const std::vector<Stmt> &stmts);
  auto operator()(const expr::Binary &binary) -> ObjectT;
  auto operator()(const expr::Grouping &grouping) -> ObjectT;
  auto operator()(const expr::Unary &unary) -> ObjectT;
  auto operator()(const expr::Literal &literal) -> ObjectT;
  auto operator()(const expr::Variable &variable) -> ObjectT;
  auto operator()(const expr::Assign &assign) -> ObjectT;
  auto operator()(const expr::Logical &assign) -> ObjectT;
  auto operator()(const expr::Call &call) -> ObjectT;
  auto operator()(const stmt::Expression &expression) -> void;
  auto operator()(const stmt::If &block) -> void;
  auto operator()(const stmt::Print &print) -> void;
  auto operator()(const stmt::While &print) -> void;
  auto operator()(const stmt::Var &var) -> void;
  auto operator()(const stmt::Empty &empty) -> void;
  auto operator()(const stmt::Block &block) -> void;

private:
  ErrorReporterPtr error_reporter_;
  std::shared_ptr<Environment> globals_ = std::make_shared<Environment>();
  std::shared_ptr<Environment> environment_ = globals_;
  void Execute(const Stmt &stmt);
  void ExecuteBlock(const std::vector<Stmt> &stmts, Environment environment);
  auto Evaluate(const Expr &expr) -> ObjectT;
  [[nodiscard]] static bool IsTruthy(const ObjectT &object);
  [[nodiscard]] static bool IsEqual(const ObjectT &left, const ObjectT &right);
  static void CheckNumberOperand(const Token &op, const ObjectT &operand);
  static void CheckNumberOperands(const Token &op, const ObjectT &left, const ObjectT &right);
};


#endif// LOX_ASTINTERPRETER_H
