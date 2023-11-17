
class Hello(IStringable) {
    Hello() {
    }

    ToStr() -> str {
        return "Hello, World!"
    }
}

format(a: IStringable) -> str {
    return a.ToStr()
}

let h: IStringable = Hello()
print(format(h))