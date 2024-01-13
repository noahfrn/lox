#ifndef LOX_EXPR_H
#define LOX_EXPR_H

#include "Common.h"
#include "Token.h"
#include <memory>


struct Expr
{
  virtual ~Expr() = default;
};

using ExprPtr = std::unique_ptr<Expr>;

struct Binary : Expr
{

  ExprPtr left_;
  Token op_;
  ExprPtr right_;
};

struct Grouping : Expr
{
  ExprPtr expression_;
};

struct LiteralExpr : Expr
{
  LiteralT value_;
};

struct Unary : Expr
{
  Token op_;
  ExprPtr right_;
};

using ExprT = std::variant<Binary, Grouping, LiteralExpr, Unary>;

#endif// LOX_EXPR_H
