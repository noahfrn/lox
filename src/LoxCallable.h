#ifndef LOX_LOXCALLABLE_H
#define LOX_LOXCALLABLE_H

#include "AstInterpreter.h"
#include "Common.h"

#include <chrono>
#include <vector>

struct AstInterpreter;

class LoxCallable
{
public:
  virtual ObjectT Call(AstInterpreter *interpreter, const std::vector<ObjectT> &arguments) = 0;
  virtual int Arity() const = 0;

  virtual ~LoxCallable() = default;
};

#endif// LOX_LOXCALLABLE_H