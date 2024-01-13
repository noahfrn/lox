#ifndef LOX_ASTINTERPRETER_H
#define LOX_ASTINTERPRETER_H

#include "Ast.h"
#include "ErrorReporter.h"
#include "common.h"

class RuntimeError : public std::runtime_error
{
public:
  RuntimeError(Token token, std::string_view message) : std::runtime_error{ message.data() }, token_{ std::move(token) }
  {}

  [[nodiscard]] auto GetToken() const -> const class Token & { return token_; }

private:
  class Token token_;
};

class AstInterpreter
{
public:
  AstInterpreter(ErrorReporterPtr error_reporter) : error_reporter_{ std::move(error_reporter) } {}
  void Interpret(const ExprPtr &expr);
  auto operator()(const Binary &binary) -> ObjectT;
  auto operator()(const Grouping &grouping) -> ObjectT;
  auto operator()(const Unary &unary) -> ObjectT;
  auto operator()(const Literal &literal) -> ObjectT;

private:
  ErrorReporterPtr error_reporter_;
  [[nodiscard]] auto Evaluate(const ExprPtr &expr) -> ObjectT;
  [[nodiscard]] static bool IsTruthy(const ObjectT &object);
  [[nodiscard]] static bool IsEqual(const ObjectT &left, const ObjectT &right);
  static void CheckNumberOperand(const Token &op, const ObjectT &operand);
  static void CheckNumberOperands(const Token &op, const ObjectT &left, const ObjectT &right);
};


#endif// LOX_ASTINTERPRETER_H
