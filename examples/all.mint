/*
 * Multi-line comment
 * int, float, str, List
 */

// Comment

mut var_a: int = 0
let var_b: str = "Hello"

var_a = 2

if var_a == 1 {
    print(var_b)
}
elif var_a == 2 {
    print("World")
}
else {
    print("Bye")
}

while var_a < 10 {
    print(var_a.ToStr())
    var_a = var_a + 1
}

foo(a: int, b: int) -> int {
    return a + b
}

print(foo(1, 2).ToStr())
