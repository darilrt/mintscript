
class Hello(IStringable) {
    ToStr() -> str {
        return "Hello, World!"
    }
}

let a: Hello = Hello()

print(a.ToStr())