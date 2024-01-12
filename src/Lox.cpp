#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <variant>

#include "Ast.h"
#include "AstPrinter.h"
#include "ErrorReporter.h"
#include "Lox.h"
#include "Parser.h"
#include "Scanner.h"
#include "Token.h"
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
  auto error_reporter = std::make_shared<ErrorReporter>(
    [this](int line, std::string_view where, std::string_view message) { Report(line, where, message); });
  Scanner scanner{ source, error_reporter };
  auto tokens = scanner.ScanTokens();
  Parser parser{ tokens, error_reporter };
  auto expression = parser.Parse();

  if (had_error_ || !expression) { return; }

  std::cout << std::visit(AstPrinterVisitor{}, *expression) << std::endl;
}

void Lox::Report(int line, std::string_view where, std::string_view message)
{
  std::cerr << fmt::format("[line {}] Error{}: {}", line, where, message) << '\n' << std::flush;
  had_error_ = true;
}