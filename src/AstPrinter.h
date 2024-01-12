#ifndef AST_PRINTER_H
#define AST_PRINTER_H

#include "Ast.h"
#include <fmt/core.h>
#include <sstream>
#include <string>
#include <string_view>
#include <variant>

class AstPrinterVisitor
{
public:
  auto operator()(const Binary &binary) -> std::string
  {
    return Parenthesize(binary.op.Lexeme(), { binary.left, binary.right });
  }

  auto operator()(const Grouping &binary) -> std::string { return Parenthesize("group", { binary.expression }); }

  auto operator()(const Unary &binary) -> std::string { return Parenthesize(binary.op.Lexeme(), { binary.right }); }

  auto operator()(const Literal &binary) -> std::string { return fmt::format("{}", binary.value); }

private:
  std::string Parenthesize(std::string_view name, std::vector<ExprPtr> exprs)
  {
    std::stringstream sstream{};
    sstream << '(' << name;
    for (const auto &expr : exprs) {
      sstream << ' ';
      if (expr) {
        sstream << std::visit(*this, *expr);
      } else {
        sstream << "nil";
      }
    }
    sstream << ')';
    return sstream.str();
  }
};
;


#endif// AST_PRINTER_H