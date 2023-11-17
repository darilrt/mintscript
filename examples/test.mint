
class Hello(IStringable) {
    ToStr() -> str {
        return "Hello, World!"
    }
}

class Goodbye(IStringable) {
    ToStr() -> str {
        return "Goodbye, World!"
    }
}

format(a: IStringable) -> str {
    return a.ToStr()
}

print(format(Hello()))
print(format(Goodbye()))