#ifndef LOX_AST_H
#define LOX_AST_H

#include <memory>
#include <variant>
#include "Common.h"
#include "Token.h"

namespace expr {
struct Binary;
struct Grouping;
struct Literal;
struct Unary;
struct Variable;
} // namespace expr
using Expr = std::variant<expr::Binary, expr::Grouping, expr::Literal, expr::Unary, expr::Variable>;
using ExprPtr = std::shared_ptr<Expr>;

namespace expr {
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

struct Variable {
    Token name;
};

} // namespace expr
template <typename ExprType, typename... Args>
auto MakeExpr(Args&&... args) -> ExprPtr
{
    return std::make_shared<Expr>(ExprType{std::forward<Args>(args)...});
}

namespace stmt {
struct Expression;
struct Print;
struct Var;
struct Empty;
} // namespace stmt
using Stmt = std::variant<stmt::Expression, stmt::Print, stmt::Var, stmt::Empty>;
using StmtPtr = std::shared_ptr<Stmt>;

namespace stmt {
struct Expression {
    ExprPtr expression;
};

struct Print {
    ExprPtr expression;
};

struct Var {
    Token name;
    ExprPtr initializer;
};

struct Empty {
};

} // namespace stmt
template <typename StmtType, typename... Args>
auto MakeStmt(Args&&... args) -> StmtPtr
{
    return std::make_shared<Stmt>(StmtType{std::forward<Args>(args)...});
}

#endif // LOX_AST_H
