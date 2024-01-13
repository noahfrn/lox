#ifndef LOX_AST_H
#define LOX_AST_H

#include <memory>
#include <variant>
#include "Common.h"
#include "Token.h"

struct Binary;
struct Grouping;
struct Literal;
struct Unary;

using Expr = std::variant<Binary, Grouping, Literal, Unary>;
using ExprPtr = std::shared_ptr<Expr>;

struct Binary {
    ExprPtr left;
    Token op;
    ExprPtr right;
};

struct Grouping {
    ExprPtr expression;
};

struct Literal {
    LiteralT value;
};

struct Unary {
    Token op;
    ExprPtr right;
};

template <typename ExprType, typename... Args>
auto MakeExpr(Args&&... args) -> ExprPtr
{
    return std::make_shared<Expr>(ExprType{std::forward<Args>(args)...});
}

#endif // LOX_AST_H
