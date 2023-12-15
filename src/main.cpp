#include "Lox.h"
#include <iostream>


int main(int argc, char **argv)
{
  Lox lox{};
  if (argc > 2) {
    std::cout << "Usage: lox [script]" << '\n' << std::flush;
    return 64;
  } else if (argc == 2) {
    return lox.RunFile(argv[1]) ? 0 : 65;
  } else {
    lox.RunPrompt();
  }
}
