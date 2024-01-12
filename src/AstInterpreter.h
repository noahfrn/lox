#ifndef LOX_ASTINTERPRETER_H
#define LOX_ASTINTERPRETER_H

#include "Ast.h"
#include "common.h"

class AstInterpreter
{
public:
  auto operator()(const Binary &binary) -> LiteralT;
  auto operator()(const Grouping &grouping) -> LiteralT;
  auto operator()(const Unary &unary) -> LiteralT;
  auto operator()(const Literal &literal) -> LiteralT;
};


#endif// LOX_ASTINTERPRETER_H
