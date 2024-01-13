#ifndef LOX_SCANNER_H
#define LOX_SCANNER_H

#include "Common.h"
#include "ErrorReporter.h"
#include "Token.h"
#include <string>
#include <string_view>
#include <utility>
#include <vector>

class Scanner
{
public:
  explicit Scanner(std::string_view source, ErrorReporterPtr reporter)
    : source_{ source }, error_reporter_{ std::move(reporter) }
  {}

  [[nodiscard]] std::vector<Token> ScanTokens();

private:
  std::string source_;
  ErrorReporterPtr error_reporter_;
  std::vector<Token> tokens_{};
  int start_{ 0 };
  int current_{ 0 };
  int line_{ 1 };

  [[nodiscard]] bool IsAtEnd() const;

  void ScanToken();

  char Advance();
  [[nodiscard]] bool Match(char expected);
  [[nodiscard]] char Peek() const;
  [[nodiscard]] char PeekNext() const;

  void AddToken(TokenType type);
  void AddToken(TokenType type, const LiteralT &literal);

  void String();
  void Number();
  void Identifier();
  void BlockComment();

  [[nodiscard]] static bool IsDigit(char c);
  [[nodiscard]] static bool IsAlpha(char c);
  [[nodiscard]] static bool IsAlphaNumeric(char c);
};


#endif// LOX_SCANNER_H
