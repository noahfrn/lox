#ifndef LOX_AST_H
#define LOX_AST_H

#include <memory>
#include <variant>
#include "common.h"
#include "Token.h"

class Expr {
public:
    virtual ~Expr() = default;
};

using ExprPtr = std::unique_ptr<Expr>;

struct Binary : Expr {
    Binary(ExprPtr left, Token op, ExprPtr right) : left(left ? std::move(left) : nullptr), op(op), right(right ? std::move(right) : nullptr) {}
    ExprPtr left;
    Token op;
    ExprPtr right;
};

struct Grouping : Expr {
    Grouping(ExprPtr expression) : expression(expression ? std::move(expression) : nullptr) {}
    ExprPtr expression;
};

struct Literal : Expr {
    Literal(LiteralT value) : value(value) {}
    LiteralT value;
};

struct Unary : Expr {
    Unary(Token op, ExprPtr right) : op(op), right(right ? std::move(right) : nullptr) {}
    Token op;
    ExprPtr right;
};

using ExprT = std::variant<
    Binary,
    Grouping,
    Literal,
    Unary
>;

#endif // LOX_AST_H
