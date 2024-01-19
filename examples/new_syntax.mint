
name: Type = value

func(Type) -> Type {

}

class class_name {
    func(arg: Type) -> Type {

    }
}

trait interface_name {
    func(arg: Type) -> Type {

    }
}

class HomeController(Controller) {

    [POST]
    index(Request) -> Response {
        return Response.make('Hello World');
    }

    [GET]
    index(Request) -> Response {
        return Response.make('Hello World');
    }
}

rule := 'print' '(' <args: expr_list> ')' {
    return new PrintNode(args);
}

