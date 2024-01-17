import mint.rules {
    Ast, Token, TokenType, Error, Type
}

rule := '@' <ident: Parser.ident> <stmt: Parser.func_decl> {
    apply_ast func_decl

    fn: Type = Type.get_type(stmt.name)
    
    fn.payload.add("attribute", ident.token)
}

@hola
kkc() -> int { return 1; }

rule ::= 'route' <Parser.string> <Parser.body> {
    class_name: str = route.token.replace("/", "_")

    if class_name.contains("<") {
        Error.error("Route parameters are not supported yet")
    }

    apply_ast Ast.Class {
        .token = Token(TokenType.String, class_name),
        .children = [
            body
        ]
    }
}

route "/users" {

    get "/<id>" {
        return Response("Hello, World!")
    }

    post "/" {
        return Response("Hello, World!")
    }
}

