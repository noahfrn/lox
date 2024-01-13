#ifndef LOX_ASTINTERPRETER_H
#define LOX_ASTINTERPRETER_H

#include "Ast.h"
#include "Common.h"
#include "Environment.h"
#include "ErrorReporter.h"
#include "Token.h"

#include <stdexcept>
#include <string_view>
#include <utility>
#include <vector>

class AstInterpreter
{
public:
  AstInterpreter(ErrorReporterPtr error_reporter) : error_reporter_{ std::move(error_reporter) } {}
  void Interpret(const std::vector<Stmt> &stmts);
  auto operator()(const expr::Binary &binary) -> ObjectT;
  auto operator()(const expr::Grouping &grouping) -> ObjectT;
  auto operator()(const expr::Unary &unary) -> ObjectT;
  auto operator()(const expr::Literal &literal) -> ObjectT;
  auto operator()(const expr::Variable &variable) -> ObjectT;
  auto operator()(const stmt::Expression &expression) -> ObjectT;
  auto operator()(const stmt::Print &print) -> ObjectT;
  auto operator()(const stmt::Var &var) -> ObjectT;
  auto operator()(const stmt::Empty &empty) -> ObjectT;

private:
  ErrorReporterPtr error_reporter_;
  Environment environment_{};
  void Execute(const Stmt &stmt);
  auto Evaluate(const Expr &expr) -> ObjectT;
  [[nodiscard]] static bool IsTruthy(const ObjectT &object);
  [[nodiscard]] static bool IsEqual(const ObjectT &left, const ObjectT &right);
  static void CheckNumberOperand(const Token &op, const ObjectT &operand);
  static void CheckNumberOperands(const Token &op, const ObjectT &left, const ObjectT &right);
};


#endif// LOX_ASTINTERPRETER_H
