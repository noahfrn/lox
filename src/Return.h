#ifndef LOX_RETURN_H
#define LOX_RETURN_H

#include "Common.h"
#include <exception>
#include <utility>

class Return : public std::exception
{
public:
  explicit Return(ObjectT value) : value_{ std::move(value) } {}

  [[nodiscard]] auto GetValue() const -> const ObjectT & { return value_; }

private:
  ObjectT value_;
};

#endif// LOX_RETURN_H
