#ifndef LOX_AST_H
#define LOX_AST_H

#include <memory>
#include <variant>
#include <vector>
#include "Common.h"
#include "Token.h"

namespace expr {
struct Assign;
struct Binary;
struct Call;
struct Grouping;
struct Literal;
struct Logical;
struct Unary;
struct Variable;
} // namespace expr
using Expr = std::variant<expr::Assign, expr::Binary, expr::Call, expr::Grouping, expr::Literal, expr::Logical, expr::Unary, expr::Variable>;
using ExprPtr = std::shared_ptr<Expr>;

namespace expr {
struct Assign {
    Token name;
    ExprPtr value;
};

struct Binary {
    ExprPtr left;
    Token op;
    ExprPtr right;
};

struct Call {
    ExprPtr callee;
    Token paren;
    std::vector<ExprPtr> arguments;
};

struct Grouping {
    ExprPtr expression;
};

struct Literal {
    LiteralT value;
};

struct Logical {
    ExprPtr left;
    Token op;
    ExprPtr right;
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
struct Function;
struct If;
struct Print;
struct Return;
struct Var;
struct While;
struct Empty;
struct Block;
} // namespace stmt
using Stmt = std::variant<stmt::Expression, stmt::Function, stmt::If, stmt::Print, stmt::Return, stmt::Var, stmt::While, stmt::Empty, stmt::Block>;
using StmtPtr = std::shared_ptr<Stmt>;

namespace stmt {
struct Expression {
    ExprPtr expression;
};

struct Function {
    Token name;
    std::vector<Token> params;
    std::vector<Stmt> body;
};

struct If {
    ExprPtr condition;
    StmtPtr then_branch;
    StmtPtr else_branch;
};

struct Print {
    ExprPtr expression;
};

struct Return {
    Token keyword;
    ExprPtr value;
};

struct Var {
    Token name;
    ExprPtr initializer;
};

struct While {
    ExprPtr condition;
    StmtPtr body;
};

struct Empty {
};

struct Block {
    std::vector<Stmt> statements;
};

} // namespace stmt
template <typename StmtType, typename... Args>
auto MakeStmt(Args&&... args) -> StmtPtr
{
    return std::make_shared<Stmt>(StmtType{std::forward<Args>(args)...});
}

#endif // LOX_AST_H
