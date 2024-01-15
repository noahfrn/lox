#ifndef LOX_COMMON_H
#define LOX_COMMON_H

#include "fmt/core.h"
#include <fmt/format.h>
#include <memory>
#include <string>
#include <type_traits>
#include <variant>

// Forward declaration
struct LoxCallable;
using LoxCallablePtr = std::shared_ptr<LoxCallable>;

template<typename T, typename... Args> struct Concatenator;
template<typename... Args0, typename... Args1> struct Concatenator<std::variant<Args0...>, Args1...>
{
  using type = std::variant<Args0..., Args1...>;
};

using Nil = std::monostate;
using LiteralT = std::variant<Nil, bool, double, int, std::string>;

using ObjectT = Concatenator<LiteralT, LoxCallablePtr>::type;

template<> struct fmt::formatter<ObjectT>
{
  template<typename ParseContext> constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

  template<typename FormatContext> auto format(const ObjectT &literal, FormatContext &ctx)
  {
    return std::visit(
      [&ctx](auto &&arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, LoxCallablePtr>) {
          return fmt::format_to(ctx.out(), "<Callable>");
        } else if constexpr (std::is_same_v<T, Nil>) {
          return fmt::format_to(ctx.out(), "nil");
        } else {
          return fmt::format_to(ctx.out(), "{}", arg);
        }
      },
      literal);
  }
};

#endif// LOX_COMMON_H
