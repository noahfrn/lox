#ifndef LOX_ERRORREPORTER_H
#define LOX_ERRORREPORTER_H

#include <cassert>
#include <functional>
#include <memory>
#include <iostream>
#include <string_view>
#include <utility>

class ErrorReporter
{
  using ErrorFn = std::function<void(int, std::string_view)>;

public:
  ErrorReporter() = default;
  explicit ErrorReporter(ErrorFn report_error) : report_error_{ std::move(report_error) } {}
  void Report(int line, std::string_view message) const { report_error_(line, message); }

private:
  ErrorFn report_error_{ [](int, std::string_view) {
    const auto* msg = "Error: Error reporter not set\n";
    std::cerr << msg << std::endl;
  } };
};

using ErrorReporterPtr = std::shared_ptr<ErrorReporter>;

#endif// LOX_ERRORREPORTER_H
