#ifndef LOX_ENVIRONMENT_H
#define LOX_ENVIRONMENT_H

#include "Common.h"
#include "Token.h"

#include <string>
#include <unordered_map>

class Environment
{
public:
  void Define(const std::string &name, const ObjectT &value);
  [[nodiscard]] ObjectT Get(const Token& name) const;
private:
  std::unordered_map<std::string, ObjectT> values_;
};

#endif// LOX_ENVIRONMENT_H
