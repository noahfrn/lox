import argparse
import sys
from enum import Enum
from io import StringIO
from typing import Union


class ExpressionType(Enum):
    BINARY = "Binary"
    GROUPING = "Grouping"
    LITERAL = "Literal"
    UNARY = "Unary"
    VARIABLE = "Variable"


class StatementType(Enum):
    EXPRESSION = "Expression"
    PRINT = "Print"
    VAR = "Var"
    EMPTY = "Empty"


class FieldType(Enum):
    EXPRESSION = "Expr"
    TOKEN = "Token"
    LITERAL = "LiteralT"


Ast = dict[Union[ExpressionType, StatementType], list[tuple[FieldType, str]]]
EXPR_AST: Ast = {
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
    ExpressionType.UNARY: [(FieldType.TOKEN, "op"), (FieldType.EXPRESSION, "right"),
                           ],
    ExpressionType.VARIABLE: [(FieldType.TOKEN, "name")],
}
STMT_AST: Ast = {
    StatementType.EXPRESSION: [(FieldType.EXPRESSION, "expression")],
    StatementType.PRINT: [(FieldType.EXPRESSION, "expression")],
    StatementType.VAR: [(FieldType.TOKEN, "name"), (FieldType.EXPRESSION, "initializer")],
    StatementType.EMPTY: [],
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
    includes.write("#include \"Common.h\"\n")
    includes.write("#include \"Token.h\"\n")
    includes.write("\n")
    return includes.getvalue()


def generate_footer() -> str:
    return "#endif // LOX_AST_H\n"


def generate_declarations(ast: Ast, name: str) -> str:
    declarations = StringIO()
    declarations.write(f"namespace {name.lower()} {{\n")
    for key in ast.keys():
        declarations.write(f"struct {key.value};\n")
    declarations.write(f"}} // namespace {name.lower()}\n")
    return declarations.getvalue()


def generate_types(ast: Ast, name: str) -> str:
    expr_types = StringIO()
    names = ', '.join([f"{name.lower()}::{str(key.value)}" for key in ast.keys()])
    expr_types.write(f"using {name} = std::variant<{names}>;\n")
    expr_types.write(f"using {name}Ptr = std::shared_ptr<{name}>;\n")
    expr_types.write("\n")
    return expr_types.getvalue()


def generate_structs(ast: Ast, name: str) -> str:
    structs = StringIO()
    structs.write(f"namespace {name.lower()} {{\n")
    for key, fields in ast.items():
        structs.write(f"struct {key.value} {{\n")
        for field_type, field_name in fields:
            structs.write(f"    {generate_type(field_type)} {field_name};\n")
        structs.write("};\n")
        structs.write("\n")
    structs.write(f"}} // namespace {name.lower()}\n")
    return structs.getvalue()


def generate_helpers(name: str) -> str:
    helpers = StringIO()
    helpers.write(f"template <typename {name}Type, typename... Args>\n")
    helpers.write(f"auto Make{name}(Args&&... args) -> {name}Ptr\n")
    helpers.write(f"{{\n")
    helpers.write(f"    return std::make_shared<{name}>({name}Type{{std::forward<Args>(args)...}});\n")
    helpers.write(f"}}\n")
    helpers.write("\n")
    return helpers.getvalue()


def generate(ast: Ast, name: str) -> str:
    buffer = StringIO()
    buffer.write(generate_declarations(ast, name))
    buffer.write(generate_types(ast, name))
    buffer.write(generate_structs(ast, name))
    buffer.write(generate_helpers(name))
    return buffer.getvalue()


def main(output: str) -> None:
    ast = StringIO()

    ast.write(generate_header())
    ast.write(generate_includes())
    ast.write(generate(EXPR_AST, "Expr"))
    ast.write(generate(STMT_AST, "Stmt"))
    ast.write(generate_footer())

    if output == "-":
        sys.stdout.write(ast.getvalue())
    else:
        with open(output, "w") as f:
            f.write(ast.getvalue())


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        prog="Lox AST generator",
    )
    parser.add_argument("output_path", nargs='?', default="-")
    args = parser.parse_args()
    main(args.output_path)
