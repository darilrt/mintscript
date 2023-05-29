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
    print(var_a)
    var_a = var_a + 1
}

foo(a: int, b: int) -> int {
    ret a + b
}

print(foo(1, 2))

// List

mut list_a : List = [1, 2, 3]

list_a.Push(4)
list_a.Push(5)

print(list_a.Pop())

print(list_a[0])
print(list_a.Length())

mut a : str = input("Name: ")

print(a)