
Animal:: concepts {
    say_hello:: fn() -> str
}

Stringable:: interface {
    to_str:: fn() -> str
}

Foo:: class(Stringable, Animal) {
    new:: fn() -> Foo {
        return {}
    }

    say_hello:: fn() -> str {
        return "Hello, from Foo!"
    }

    to_str:: fn() -> str {
        return "Foo()"
    }
}

main:: fn() {
    foo: Foo = Foo.new()

    foo.say_hello()

    foo.to_str()
}