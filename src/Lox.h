#ifndef LOX_LOX_H
#define LOX_LOX_H

#include "ErrorReporter.h"
#include <string_view>

class Lox
{
public:
  bool RunFile(std::string_view path);
  [[noreturn]] void RunPrompt();

private:
  bool had_error_{ false };

  void Report(int line, std::string_view where, std::string_view message);
  void Run(std::string_view source);
};


#endif// LOX_LOX_H
