#ifndef LOX_ERRORREPORTER_H
#define LOX_ERRORREPORTER_H

#include <cassert>
#include <functional>
#include <iostream>
#include <memory>
#include <string_view>
#include <utility>

class ErrorReporter
{
  using ErrorFn = std::function<void(int, std::string_view, std::string_view)>;
  using RuntimeErrorFn = std::function<void(int, std::string_view)>;

  static void DefaultErrorReporter(int line, std::string_view where, std::string_view message)
  {
    std::cerr << "[line " << line << "] Error" << where << ": " << message << std::endl;
  }
  static void DefaultRuntimeErrorReporter(int line, std::string_view message)
  {
    std::cerr << "[line " << line << "] Error: " << message << std::endl;
  }

public:
  ErrorReporter() = default;
  explicit ErrorReporter(ErrorFn report_error, RuntimeErrorFn runtime_report_error)
    : report_error_{ std::move(report_error) }, runtime_report_error_{ std::move(runtime_report_error) }
  {}
  void Report(int line, std::string_view where, std::string_view message) const { report_error_(line, where, message); }
  void ReportRuntime(int line, std::string_view message) const { runtime_report_error_(line, message); }

private:
  ErrorFn report_error_{ DefaultErrorReporter };
  RuntimeErrorFn runtime_report_error_{ DefaultRuntimeErrorReporter };
};

using ErrorReporterPtr = std::shared_ptr<ErrorReporter>;

#endif// LOX_ERRORREPORTER_H
