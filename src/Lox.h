#ifndef LOX_LOX_H
#define LOX_LOX_H

#include "AstInterpreter.h"
#include "ErrorReporter.h"
#include <memory>
#include <string_view>

class Lox
{
public:
  bool RunFile(std::string_view path);
  [[noreturn]] void RunPrompt();

private:
  bool had_error_{ false };
  bool had_runtime_error_{ false };
  ErrorReporterPtr error_reporter_{ std::make_shared<ErrorReporter>(
    [this](int line, std::string_view where, std::string_view message) { Report(line, where, message); },
    [this](int line, std::string_view message) { ReportRuntime(line, message); }) };
  AstInterpreter interpreter_{ error_reporter_ };

  void Report(int line, std::string_view where, std::string_view message);
  void ReportRuntime(int line, std::string_view message);
  void Run(std::string_view source);

  [[nodiscard]] bool HadError() const;
};


#endif// LOX_LOX_H
