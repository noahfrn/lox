#ifndef LOX_CLOCKCALLABLE_H
#define LOX_CLOCKCALLABLE_H

#include "Common.h"
#include "LoxCallable.h"

class AstInterpreter;

class ClockCallable : public LoxCallable
{
public:
  ObjectT Call([[maybe_unused]] AstInterpreter *interpreter,
    [[maybe_unused]] const std::vector<ObjectT> &arguments) override
  {
    auto millis =
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
    return static_cast<double>(millis.count()) / 1000.0;
  };

  [[nodiscard]] int Arity() const override { return 0; };
};


#endif// LOX_CLOCKCALLABLE_H
