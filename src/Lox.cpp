#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>


#include "Lox.h"
#include "Parser.h"
#include "Scanner.h"
#include "fmt/core.h"

bool Lox::RunFile(std::string_view path)
{
  std::ifstream file{ path.data() };
  if (!file) { throw std::runtime_error(fmt::format("File not found: {}", path)); }

  std::stringstream buffer;
  buffer << file.rdbuf();
  file.close();

  Run(buffer.str());

  return had_error_;
}


void Lox::RunPrompt()
{
  std::string line{};
  while (true) {
    std::cout << "> " << std::flush;
    getline(std::cin, line);
    Run(line);
    had_error_ = false;
  }
}

void Lox::Run(std::string_view source)
{
  Scanner scanner{ source, error_reporter_ };
  auto tokens = scanner.ScanTokens();
  Parser parser{ tokens, error_reporter_ };
  auto expression = parser.Parse();

  if (HadError() || !expression) { return; }

  interpreter_.Interpret(expression);
}

void Lox::Report(int line, std::string_view where, std::string_view message)
{
  std::cerr << fmt::format("[line {}] Error{}: {}", line, where, message) << '\n' << std::flush;
  had_error_ = true;
}

void Lox::ReportRuntime(int line, std::string_view message)
{
  std::cerr << fmt::format("[line {}] Error: {}", line, message) << '\n' << std::flush;
  had_runtime_error_ = true;
}

bool Lox::HadError() const { return had_error_ || had_runtime_error_; }