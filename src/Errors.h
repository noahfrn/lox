#ifndef LOX_ERRORS_H
#define LOX_ERRORS_H

#include "Token.h"

#include <stdexcept>
#include <string_view>

class RuntimeError : public std::runtime_error
{
public:
  RuntimeError(Token token, std::string_view message) : std::runtime_error{ message.data() }, token_{ std::move(token) }
  {}

  [[nodiscard]] auto GetToken() const -> const class Token & { return token_; }

private:
  class Token token_;
};


class ParseError : public std::runtime_error
{
public:
  ParseError(std::string_view message) : std::runtime_error{ message.data() } {}
};


#endif// LOX_ERRORS_H
