# Mintscript

[GitHub](https://github.com/darilrt/mintscript.git)

## Introduction

Mintscript is a programming language that intends to be easy to use but still powerful. It is a interpreted language but its statical typed.

## Hello World

First of all, lets start with the classic Hello World program. In Mintscript it looks like this:

```mintscript
main() {
    print("Hello World!")
}
```

Save this code in a file called `hello.mint` and run it with the command `mint hello.mint`. You should see the output `Hello World!`.

## Variables and Types

Mintscript is statically typed, which means that you have to declare the type of a variable. The types are:

| Type | Description |
| ---- | ----------- |
| `int` | A integer number |
| `float` | A floating point number |
| `bool` | A boolean value |
| `str` | A string of characters |

In mintscript you have to types of variables, mutable and immutable. A mutable variable can be changed after it has been declared, while a immutable variable can not be changed after it has been declared. To declare a inmutable variable you use the `let` keyword, while to declare a mutable variable you use the `mut` keyword. Here is an example of how to declare a variable:

```mintscript
let a: int = 5
mut b: int = 10
```

You can also declare multiple variables and infer the type of them:

```mintscript
let a := 5
mut b := 10
```

### Type Conversion

You can convert between types by using the `as` keyword:

```mintscript
let a: int = 5
let b: float = a as float
```

## Arrays

Arrays are object of class `List`. You can create a list by using the `[]` syntax:

```mintscript
// Inferred type
let a := [1, 2, 3]
// Explicit type
let b: [..] int = [1, 2, 3]
```

If you use the `..` syntax, the list will be dynamic, which means that you can add and remove elements from it. If you dont use the `..` syntax, the list will be static, which means that you can not add or remove elements from it. You can access the elements of a list by using the `[]` syntax:

```mintscript
let b := a[0] // 1
```

You can also use the `[]` syntax to set the value of an element:

```mintscript
a[0] = 5
```

## Functions

Functions are declared without any keyword. The return type is declared after the `->` symbol. Here is an example of a function:

```mintscript
add(a: int, b: int) -> int {
    return a + b
}
```

The `return` keyword is used to return a value from a function. You can also return early from a function by using the `return` keyword without any value. You can also return values without using the `return` keyword by just writing the value you want to return:

```mintscript
add(a: int, b: int) -> int { 
    a + b
}
// Is the same as
add(a: int, b: int) -> int { 
    return a + b
}
// or just
add(a: int, b: int) -> int { return a + b }
```

If you dont want to return anything from a function you can declare it as `void` or just leave out the return type:

```mintscript
foo() -> void { ... }
// or
foo() { ... }
```

All functions are stored as anonymous functions in a variable. So you can also declare a function like this:

```mintscript
// using the type inference
let add := (a: int, b: int) -> int { a + b }
// or not
let add: (int, int) -> int = (a: int, b: int) -> int { a + b }
```

So you can pass functions as arguments to other functions:

```mintscript
add(a: int, b: int) -> int { a + b }

apply(f: (int, int) -> int, a: int, b: int) -> int {
    return f(a, b)
}

apply(add, 5, 10) // 15
```

The functions accept named arguments. This means that you can pass the arguments in any order you want:

```mintscript
add(a: int, b: int) -> int { a + b }

add(b: 5, a: 10) // 15
```

Also you can define default values for arguments:

```mintscript
add(a: int, b: int = 5) -> int { a + b }
add(10) // 15
add(a: 10) // 15
```

## Classes

Classes are declared with the `class` keyword. Here is an example of a class:

```mintscript
class Point {
    mut x: int
    mut y: int

    mInit(x: int, y: int) {
        .x = x
        .y = y
    }

    // operator overloading
    mAdd(other: Point) -> Point { Point(.x + other.x, .y + other.y) }
}
```

## Control Flow

The control flow in Mintscript is pretty simple. You have the `if` statement, the `while` loop and the `for` loop, but additionally you have the `elif` and `match` statament. This is how they look like:

```mintscript
// if statement
if condition {
    ...
} elif condition {
    ...
} else {
    ...
}

// while loop
while condition { ... }

// for is a foreach loop
// and have different syntaxes

// defining a type and mutability
for let item : type in list { ... }

// defining only a mutability
for let item in list { ... }

// defining only a type
for item : type in list { ... }

// defining nothing
for item in list { ... }

// match statement
match value {
    case pattern -> { ... }
    case pattern -> { ... }
    ...
}
```

Some of these statements can be used as expressions. For example the `if` statement or the `match` statement. This means that you can assign the result of them to a variable:

```mintscript
let a := if condition { 5 } else { 10 }

let b := match value {
    case pattern -> { 5 }
    case pattern -> { 10 }
}
```

## Modules

Mintscript has a module system. You can import other modules with the `import` keyword. The syntax for importing a module is:

```mintscript
import path.to.module
```

This will import the module `module` from the path `path.to`. You can also import a module with a different name:

```mintscript
import path.to.module as my_module
```

This will import the module `module` from the path `path.to` and store it in the variable `my_module`. You can also import all the functions from a module into the current scope:

```mintscript
import path.to.module.*
```

A module is just a folder with a `root.mint` file in it. The `root.mint` file is the entry point of the module. So you can import a file directly:

```mintscript
import './path/to/file.mint'
```

When you import a file or module, the code in it will be executed and the keywords that are exported will be exported. You can export a keyword by using the `export` keyword:

```mintscript
let a := 5
let b := 10

export { a, b }
```

This will export the variables `a` and `b` from the module. You can also export with a different name:

```mintscript
export { a as my_a, b as my_b }
```

Also import keywords with a different name:

```mintscript
import path.to.module { a as my_a, b as my_b }
```

## Generics Types

Mintscript has generic types. You can declare a generic type by using the `[]` syntax in function declarations or class declarations:

```mintscript
add[T](a: T, b: T) -> T { a + b }
```

This will declare a function that takes two arguments of the same type and returns a value of the same type. You can also declare multiple generic types:

```mintscript
add[T, U](a: T, b: U) -> T { a + b }
```

## Examples

You can find some examples in the `examples` folder.
