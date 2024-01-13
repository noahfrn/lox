#include "Environment.h"
#include "Common.h"
#include "Errors.h"
#include "Token.h"

#include <fmt/core.h>
#include <string>

void Environment::Define(const std::string &name, const ObjectT &value) { values_[name] = value; }

ObjectT Environment::Get(const Token &name) const
{
  if (values_.contains(name.Lexeme())) { return values_.at(name.Lexeme()); }

  throw RuntimeError(name, fmt::format("Undefined variable '{}'.", name.Lexeme()));
}

void Environment::Assign(const Token &name, const ObjectT &value)
{
  if (values_.contains(name.Lexeme())) {
    values_[name.Lexeme()] = value;
    return;
  }

  throw RuntimeError(name, fmt::format("Undefined variable '{}'.", name.Lexeme()));
}