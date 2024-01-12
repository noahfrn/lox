//
// Created by Noah Fournier on 12/01/2024.
//

#ifndef LOX_COMMON_H
#define LOX_COMMON_H

#include <fmt/format.h>
#include <variant>

using LiteralT = std::variant<std::monostate, bool, double, int, std::string>;

template<> struct fmt::formatter<LiteralT>
{
  template<typename ParseContext> constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

  template<typename FormatContext> auto format(const LiteralT &literal, FormatContext &ctx)
  {
    return std::visit(
      [&ctx](auto &&arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::monostate>) {
          return fmt::format_to(ctx.out(), "nil");
        } else {
          return fmt::format_to(ctx.out(), "{}", arg);
        }
      },
      literal);
  }
};


#endif// LOX_COMMON_H
