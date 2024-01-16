#ifndef LOX_LOXFUNCTION_H
#define LOX_LOXFUNCTION_H

#include "Ast.h"
#include "AstInterpreter.h"
#include "Common.h"
#include "LoxCallable.h"
#include "Return.h"

#include <memory>
#include <vector>

class LoxFunction : public LoxCallable
{
public:
  LoxFunction(stmt::Function declaration, std::shared_ptr<Environment> closure)
    : declaration_{ std::move(declaration) }, closure_{ std::move(closure) }
  {}
  ObjectT Call(AstInterpreter *interpreter, const std::vector<ObjectT> &arguments) override
  {
    auto environment = std::make_shared<Environment>(closure_);
    for (int i = 0; i < declaration_.params.size(); ++i) {
      environment->Define(declaration_.params[i].Lexeme(), arguments.at(i));
    }

    try {
      interpreter->ExecuteBlock(declaration_.body, *environment);
    } catch (const Return &return_value) {
      return return_value.GetValue();
    }

    return Nil{};
  }

  [[nodiscard]] int Arity() const override { return declaration_.params.size(); }


private:
  stmt::Function declaration_;
  std::shared_ptr<Environment> closure_;
};

#endif// LOX_LOXFUNCTION_H
