//
// Created by Noah Fournier on 15/12/2023.
//

#ifndef LOX_LOX_H
#define LOX_LOX_H

#include <string_view>

class Lox
{
public:
  bool RunFile(std::string_view path);
  [[noreturn]] void RunPrompt();

private:

  bool had_error;
  void Report(int line, std::string_view where, std::string_view message);
  void Error(int line, std::string_view message);
  int Run(std::string_view source);
};


#endif// LOX_LOX_H
