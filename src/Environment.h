#ifndef LOX_ENVIRONMENT_H
#define LOX_ENVIRONMENT_H

#include "Common.h"
#include "Token.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

class Environment
{
public:
  Environment() = default;
  explicit Environment(std::shared_ptr<Environment> enclosing) : enclosing_{ std::move(enclosing) } {}

  void Define(const std::string &name, const ObjectT &value);
  [[nodiscard]] ObjectT Get(const Token &name) const;
  void Assign(const Token &name, const ObjectT &value);

private:
  std::unordered_map<std::string, ObjectT> values_;
  std::shared_ptr<Environment> enclosing_{ nullptr };
};

#endif// LOX_ENVIRONMENT_H
