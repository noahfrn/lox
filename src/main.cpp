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
    bool debug = false;
    std::string script{};

    // clang-format off
    auto cli
      = lyra::cli()
      | lyra::opt( debug )
          ["-d"]["--debug"]
          ("Enable debug logging.")
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
      lox.RunPrompt();
    } else {
      lox.RunFile(script);
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
