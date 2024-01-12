//
// Created by Noah Fournier on 12/01/2024.
//

#ifndef LOX_COMMON_H
#define LOX_COMMON_H

#include <variant>

using LiteralT = std::variant<std::monostate, bool, double, std::string>;

#endif// LOX_COMMON_H
