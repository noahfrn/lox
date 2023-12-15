#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>

#include "Lox.h"
#include "Scanner.h"
#include "fmt/format.h"

bool Lox::RunFile(std::string_view path)
{
  std::ifstream file{ path };
  if (!file) { throw std::runtime_error(fmt::format("File not found: {}", path)); }

  std::stringstream buffer;
  buffer << file.rdbuf();
  file.close();

  Run(buffer.str());

  return had_error;
}


void Lox::RunPrompt()
{

  std::string line{};
  while (true) {
    std::cout << "> " << std::flush;
    getline(std::cin, line);
    Run(line);
    had_error = false;
  }
}

int Lox::Run(std::string_view source)
{
  Scanner scanner{ source };
  auto tokens = scanner.ScanTokens();

  for (const auto& token : tokens) {
    fmt::println("{}", token);
  }
}


void Lox::Report(int line, std::string_view where, std::string_view message)
{
  fmt::println(stderr, "[line {}] Error {}: {}", line, where, message);
  had_error = true;
}

void Lox::Error(int line, std::string_view message) { Report(line, "", message); }
