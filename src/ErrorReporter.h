#ifndef LOX_ERRORREPORTER_H
#define LOX_ERRORREPORTER_H

#include <cassert>
#include <fmt/format.h>
#include <iostream>
#include <functional>
#include <string_view>

class ErrorReporter
{
  using ErrorLambda = std::function<void(int, std::string_view)>;

public:
  ErrorReporter() = default;
  explicit ErrorReporter(ErrorLambda report_error) : report_error_{ std::move(report_error) } {}
  void Report(int line, std::string_view message) const { report_error_(line, message); }

private:
  ErrorLambda report_error_{ [](int, std::string_view) {
    std::cerr << fmt::format("Error: {}\n", "Error reporter not set") << '\n' << std::flush;
  } };
};


#endif// LOX_ERRORREPORTER_H
