import argparse
import sys
from enum import Enum
from io import StringIO


class ExpressionType(Enum):
    BINARY = "Binary"
    GROUPING = "Grouping"
    LITERAL = "Literal"
    UNARY = "Unary"


class FieldType(Enum):
    EXPRESSION = "Expr"
    TOKEN = "Token"
    LITERAL = "LiteralT"


Type = dict[ExpressionType, list[tuple[FieldType, str]]]
AST = {
    ExpressionType.BINARY: [
        (FieldType.EXPRESSION, "left"),
        (FieldType.TOKEN, "op"),
        (FieldType.EXPRESSION, "right"),
    ],
    ExpressionType.GROUPING: [
        (FieldType.EXPRESSION, "expression"),
    ],
    ExpressionType.LITERAL: [
        (FieldType.LITERAL, "value"),
    ],
    ExpressionType.UNARY: [
        (FieldType.TOKEN, "op"),
        (FieldType.EXPRESSION, "right"),
    ],
}


def shared_ptr(val: str) -> str:
    return f"std::shared_ptr<{val}>"


def unique_ptr(val: str) -> str:
    return f"std::unique_ptr<{val}>"


def generate_type(field_type: FieldType) -> str:
    if field_type == FieldType.EXPRESSION:
        return "ExprPtr"
    elif field_type == FieldType.TOKEN:
        return "Token"
    elif field_type == FieldType.LITERAL:
        return "LiteralT"
    else:
        raise ValueError(f"Unknown field type: {field_type}")


def generate_header() -> str:
    header = StringIO()
    header.write("#ifndef LOX_AST_H\n")
    header.write("#define LOX_AST_H\n")
    header.write("\n")
    return header.getvalue()


def generate_includes() -> str:
    includes = StringIO()
    includes.write("#include <memory>\n")
    includes.write("#include <variant>\n")
    includes.write("#include \"common.h\"\n")
    includes.write("#include \"Token.h\"\n")
    includes.write("\n")
    return includes.getvalue()


def generate_footer() -> str:
    return "#endif // LOX_AST_H\n"


def generate_declarations() -> str:
    declarations = StringIO()
    for expr_type in AST.keys():
        declarations.write(f"struct {expr_type.value};\n")
    declarations.write("\n")
    return declarations.getvalue()


def generate_expr_types() -> str:
    expr_types = StringIO()
    names = ', '.join([expr_type.value for expr_type in AST.keys()])
    expr_types.write(f"using Expr = std::variant<{names}>;\n")
    expr_types.write("using ExprPtr = std::shared_ptr<Expr>;\n")
    expr_types.write("\n")
    return expr_types.getvalue()


def generate_structs() -> str:
    structs = StringIO()
    for expr_type, fields in AST.items():
        structs.write(f"struct {expr_type.value} {{\n")
        for field_type, field_name in fields:
            structs.write(f"    {generate_type(field_type)} {field_name};\n")
        structs.write("};\n")
        structs.write("\n")
    return structs.getvalue()


def generate_helpers() -> str:
    helpers = StringIO()
    helpers.write(f"template <typename ExprType, typename... Args>\n")
    helpers.write(f"auto MakeExpr(Args&&... args) -> ExprPtr\n")
    helpers.write(f"{{\n")
    helpers.write(f"    return std::make_shared<Expr>(ExprType{{std::forward<Args>(args)...}});\n")
    helpers.write(f"}}\n")
    helpers.write("\n")
    return helpers.getvalue()


def generate_ast() -> str:
    ast = StringIO()

    ast.write(generate_header())
    ast.write(generate_includes())
    ast.write(generate_declarations())
    ast.write(generate_expr_types())
    ast.write(generate_structs())
    ast.write(generate_helpers())
    ast.write(generate_footer())

    return ast.getvalue()


def main(output: str) -> None:
    ast = generate_ast()
    if output == "-":
        sys.stdout.write(ast)
    else:
        with open(output, "w") as f:
            f.write(ast)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        prog="Lox AST generator",
    )
    parser.add_argument("output_path", nargs='?', default="-")
    args = parser.parse_args()
    main(args.output_path)
