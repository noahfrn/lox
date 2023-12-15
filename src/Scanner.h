#ifndef LOX_SCANNER_H
#define LOX_SCANNER_H

#include "Token.h"
#include <string>
#include <string_view>
#include <vector>

class Scanner
{
public:
  explicit Scanner(std::string_view source)
   : source_{source} {}

  [[nodiscard]] std::vector<Token> ScanTokens() const;

private:
  std::string source_;
};


#endif// LOX_SCANNER_H
