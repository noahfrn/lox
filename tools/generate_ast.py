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


def generate_init(field_type: FieldType, field_name: str) -> str:
    if field_type == FieldType.EXPRESSION:
        return f"{field_name} ? std::move({field_name}) : nullptr"
    else:
        return field_name


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


def generate_base_class() -> str:
    base_class = StringIO()
    base_class.write("class Expr {\n")
    base_class.write("public:\n")
    base_class.write("    virtual ~Expr() = default;\n")
    base_class.write("};\n")
    base_class.write("\n")
    return base_class.getvalue()


def generate_exprptr_type() -> str:
    expr_type = StringIO()
    expr_type.write("using ExprPtr = std::unique_ptr<Expr>;\n")
    expr_type.write("\n")
    return expr_type.getvalue()


def generate_struct(expr_type: ExpressionType, fields: list[tuple[FieldType, str]]) -> str:
    struct = StringIO()
    struct.write(f"struct {expr_type.value} : Expr {{\n")

    # Constructor
    constructor_args = ', '.join(
        f"{generate_type(field_type)} {field_name}" for field_type, field_name in fields)
    constructor_initializers = ', '.join(
        f"{field_name}({generate_init(field_type, field_name)})" for field_type, field_name in fields)
    struct.write(f"    {expr_type.value}({constructor_args}) : {constructor_initializers} {{}}\n")

    # Fields
    field_declarations = '\n'.join(
        f"    {generate_type(field_type)} {field_name};" for field_type, field_name in fields)
    struct.write(f"{field_declarations}\n")

    struct.write("};\n\n")
    return struct.getvalue()


def generate_footer() -> str:
    return "#endif // LOX_AST_H\n"


def generate_body() -> str:
    body = StringIO()

    for expr_type, fields in AST.items():
        body.write(generate_struct(expr_type, fields))

    return body.getvalue()


def generate_expr_variant() -> str:
    variant = StringIO()
    variant.write("using ExprT = std::variant<\n")
    keys = list(AST.keys())
    for expr_type in keys[:-1]:
        variant.write(f"    {expr_type.value},\n")
    variant.write(f"    {keys[-1].value}\n")
    variant.write(">;\n")
    variant.write("\n")
    return variant.getvalue()


def generate_ast() -> str:
    ast = StringIO()

    ast.write(generate_header())
    ast.write(generate_includes())
    ast.write(generate_base_class())
    ast.write(generate_exprptr_type())
    ast.write(generate_body())
    ast.write(generate_expr_variant())
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
