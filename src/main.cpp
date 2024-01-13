#include "Lox.h"
#include "fmt/core.h"
#include <cstdlib>
#include <exception>
#include <iostream>
#include <lyra/lyra.hpp>
#include <string>


int main(int argc, char **argv)
{
  try {
    std::string script{};

    // clang-format off
    auto cli
      = lyra::cli()
      | lyra::arg( script, "script" )
          ("Script to run.");
    // clang-format on

    auto result = cli.parse({ argc, argv });
    if (!result) {
      auto msg = fmt::format("Error parsing command line: {}", result.message());
      std::cerr << msg << std::endl;// nolint
      return EXIT_FAILURE;
    }

    Lox lox{};
    if (script.empty()) {
      // REPL
      lox.RunPrompt();
    } else {
      return lox.RunFile(script) ? EXIT_FAILURE : EXIT_SUCCESS;
    }
  } catch (std::exception &e) {
    auto msg = fmt::format("Caught identified exception: {}", e.what());
    std::cerr << msg << std::endl;// nolint
    return EXIT_FAILURE;
  } catch (...) {
    std::cerr << "Unidentified exception thrown." << std::endl;// nolint
    return EXIT_FAILURE;
  }
}
