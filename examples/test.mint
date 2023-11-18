
class Foo(IStringable) {
    ToStr() -> str { "Foo" }
}

class Boo(IStringable) {
    ToStr() -> str { "Boo" }
}

myPrint(obj: IStringable) -> void {
    print(obj.ToStr())
}

myPrint(Foo()) 
myPrint(Boo())
myPrint(10) 

// Output:
// Foo
// Boo
// 10
