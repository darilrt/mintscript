# Mintscript

## Introduction

### Variable declaration and assignment

The following code declares a variable named `x` as an integer and assigns it the value `5`:

```mintscript
x: int = 5
```

You can also infer the type of a variable from its value:

```mintscript
x := 5
```

### Nullability

Variables can be declared as nullable by adding a `?` after the type:

```mintscript
x: int? = null
```

it declares a variable named `x` as an integer that can be `null` or an `Optional<int>`.
The same can be achieved by using the `Optional` type:

```mintscript
x: Optional[int] = null
```

### Variable reassignment

Variables can be reassigned by using the `=` operator:

```mintscript
x = 10
```

### Variable scope

Variables are scoped to the block they are declared in:

```mintscript
x := 5
if x == 5 {
    y := 10
    print(y) // 10
}
print(y) // Error: y is not defined
```

### Variable shadowing

Variables can be shadowed by redeclaring them in a nested scope:

```mintscript
x := 5
if x == 5 {
    x := 10
    print(x) // 10
}
print(x) // 5
```

### Variable redeclaration

Variables can be redeclared in the same scope:

```mintscript
x := 5
print(x) // 5
x := "Hello"
print(x) // "Hello"
```

### Function declaration

Functions can be declared without keywords only a label followed by a parameter list return type after '->' and a block:

```mintscript
add(a: int, b: int) -> int {
    return a + b
}
```

If the function not returns a value, the return type can be omitted:

```mintscript
print_hello() {
    print("Hello")
}
```

you can call a function by using its label followed by a parameter list:

```mintscript
add(5, 10) // 15
print_hello() // "Hello"
```

### Function overloading

Functions cannot be overloaded, but you can use default parameters to achieve the same result:

```mintscript
add(a: int, b: int = 5) -> int {
    return a + b
}
add(5) // 10
add(5, 10) // 15
```

### Lambda expressions

Lambda expressions are anonymous functions that can be assigned to variables:

```mintscript
add := (a: int, b: int) -> int {
    return a + b
}
// or with type annotation
add: (int, int) -> int = (a: int, b: int) -> int {
    return a + b
}
// or
add: Function[int, int, int] = (a: int, b: int) -> int {
    return a + b
}
```

### Arrays

Arrays can be declared by using the `[]` operator before the type:

```mintscript
x: [] int = [1, 2, 3]
// or with type inference
x := [1, 2, 3]
// with template type
x: List[int] = [1, 2, 3]
```

Arrays can be accessed and modified by using the `[]` operator:

```mintscript
x := [1, 2, 3]
x[0] = 5
print(x[0]) // 5
```

### Tuples

Tuples can be declared by using the `()` operator before the type:

```mintscript
x: (int, int) = (1, 2)
// or with type inference
x := (1, 2)
// with template type
x: Tuple[int, int] = (1, 2)
```

### Dictionaries

Dictionaries can be declared by using the `{}` operator before the type:

```mintscript
x: {str: int} = {"a": 1, "b": 2}
// or with type inference
x := {"a": 1, "b": 2}
// with template type
x: Map[str, int] = {"a": 1, "b": 2}
```

### If, elif, else and while

The common `if`, `elif`, `else` and `while` statements are supported:

```mintscript
x := 5
if x == 5 {
    print("x is 5")
} elif x == 10 {
    print("x is 10")
} else {
    print("x is not 5 or 10")
}

while x < 10 {
    print(x)
    x += 1
}
```

### For

The `for` each statement is supported:

```mintscript
x := [1, 2, 3]

for i in x {
    print(i)
}
```

the `for` works with any iterable that implements the `Iterator` interface:

```mintscript
x := "Hello"

for i in x {
    print(i)
}
```

### Classes

Classes can be declared by using the `class` keyword followed by the class name and the interface list:

```mintscript
class Person(Human, Animal) {
    name: str
    age: int

    Person(name: str, age: int) {
        this.name = name
        this.age = age
    }

    say_hello() {
        print("Hello, my name is " + .name) // .name is the same as this.name
    }
}
```

Im the methods its obligatory to use the `this` keyword to access the class members or the `.` operator to access the class members from the current scope.
Classes can be instantiated by the class name and the constructor parameters:

```mintscript
person := Person("John", 20)
person.say_hello() // "Hello, my name is John"
```

### Interfaces

Interfaces can be declared by using the `interface` keyword followed by the interface name and the method list:

```mintscript
interface Human {
    say_hello() -> void
}
```

Compounds interfaces are supported:

```mintscript
interface Human {
    say_hello() -> void
}

interface Animal {
    say_hello() -> void
}

interface HumanAnimal(Human, Animal) {
    say_hello() -> void
}
```

### Imports

Every file contains exports and imports. The exports are defined by the `export` keyword followed by the exported variable name and the exported variable or value:

```mintscript
x := 5
export {
    x
}
```

The imports are defined by the `import` keyword followed by the imported variable name and the imported variable or value:

```mintscript
import { x } from "file.mint"
```

You can also import all the exported variables by using the `*` operator:

```mintscript
import * from "file.mint"
```

### Modules

Modules are folders that contain a `root.mint` file that is the entry point of the module. The `root.mint` file can import other files from the same module, but it cannot import files from other modules. All exported from the `root.mint` file are exported from the module. And the name of the module is the name of the folder that contains the `root.mint` file.

To import a module you can use the `import` keyword followed by the module name and the imported variable name:

```mintscript
import module_name.*
import module_name.x
import module_name { x, y }
```

### Comments

Comments can be declared by using the `//` operator:

```mintscript
// This is a comment
```

### Generics

Generics can be declared by using the `[]` operator after the type.
Example, to define a generic function that returns the first element of an array:

```mintscript
first[T](arr: [] T) -> T {
    return arr[0]
}
```

You can also define a constraint for the generic:

```mintscript
first[T: int](arr: [] T) -> T {
    return arr[0]
}
```

The constraint can be a type or an interface and can be multiple and negated:

```mintscript
first[T: Operable & !str](arr: [] T) -> T {
    return arr[0]
}
```

### Operators

The following operators are supported and have an interface that can be implemented to overload the operator:

```mintscript
+ - * / % ** // Arithmetic
== != > < >= <= // Comparison
&& || ! // Logical
& | ^ ~ << >> // Bitwise
+= -= *= /= %= **= //= &= |= ^= ~= <<= >>= // Assignment
++ -- // Increment and decrement
[] // Index
() // Call
. // Member access
```

### Built-in types

The following built-in types are supported:

```mintscript
int
float
str
bool
null
void
List[T]
Map[T, U]
Optional[T]
Function[T, ...]
```

### Built-in functions

The following built-in functions are supported:

```mintscript
print(args: Stringable...) -> void
input(prompt: str? = null) -> str
```

### Attributes

Attributes can be declared by using the `@` operator before the attribute name:

```mintscript
// This attribute is used to mark a function as the entry point of the program
@entry_point
main() {
    print("Hello")
}
```

Every attribute has a class that can be implemented to create a custom attribute:

```mintscript
import reflect.*

class entry_point(Attribute) {
    on_function(func: Function) {
        // Do something with the function
    }

    on_class(cls: Class) {
        // Do something with the class
    }

    on_variable(var: Variable) {
        // Do something with the variable
    }
}
```

### Reflection

The reflection is supported by using the `reflect` module:

```mintscript
import reflect.*

class Person {
    name: str
    age: int

    Person(name: str, age: int) {
        this.name = name
        this.age = age
    }

    say_hello() {
        print("Hello, my name is " + .name) // .name is the same as this.name
    }
}

person := Person("John", 20)

// Get the class of the person variable
person_type := type_of(person) // Class<Person>

// Get the fields of the person class
fields := person_type.fields // [] Field

// Get the methods of the person class
methods := person_type.methods // [] Method

// Get the name of the person class
name := person_type.name // "Person"

// Get the value of the name field
name_field := fields[0]
name_value := name_field.get(person) // "John"

// Get the say_hello method
say_hello_method := methods[0]

// Call the say_hello method
say_hello_method.call(person) // "Hello, my name is John"
```

### Exceptions

Mintscript not supports exceptions, but you can use the `Result` type to handle errors:

```mintscript
add(a: int, b: int) -> Result[int] {
    if a == 0 {
        return err[int]("a cannot be 0")
    }
    return ok[int](a + b)
}

result := add(0, 5)

if result.is_err() {
    print(result.err()) // "a cannot be 0"
} else {
    print(result.ok()) // 5
}
```

### Static methods

Static methods can be declared by using the `static` keyword before the method name:

```mintscript
class Person {
    name: str
    age: int

    Person(name: str, age: int) {
        this.name = name
        this.age = age
    }

    static create(name: str, age: int) -> Person {
        return Person(name, age)
    }
}

person := Person.create("John", 20)
```

### Type aliases

Type aliases can be declared by using the `type` keyword before the type alias name:

```mintscript
type Person = (str, int)
```

### Type casting

Type casting can be done by using the `as` operator:

```mintscript
x := 5 as float
```

### Macros

The macro system is supported, you can define a rule that is identified in the code to be replaced by another code.
First define de rule with the `rule` keyword followed by the rule name and the rule code:

```mintscript
rule attr ::= 'hello' '(' <arg1: str> ')' {
    apply_ast(call_func('print', [arg1]))
}
// usage
hola("Hello, World!") // replaced by print("Hello, World!") and outputs "Hello, World!"
```

The rule notation is similar to the EBNF notation:

- alternation `|`
- optional `[ ... ]`
- repetition `{ ... }`
- grouping `( ... )`
- terminal string `" ... "`
- terminal string `' ... '`
- repeat `...*`
- repeat at least once `...+`

To store the value of a terminal string in a variable, use `<` and `>` around the variable name then `:` and the type of the variable `<name: expr>`

Example of a rule print with N arguments:

```mintscript
rule print ::= 'print' '(' <args: str> [ ',' <args: str> ]* ')' {
    apply_ast(call_func('print', [args]))
}
// usage
print("Hello", "World!") // replaced by print("Hello", "World!") and outputs "Hello World!"
```
