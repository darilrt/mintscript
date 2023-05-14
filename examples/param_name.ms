
// default parameters
foo (a : int = 1, b : int = 2) -> int {
    ret a + b
}

// call with default parameters
foo()

// call with one parameter
foo(3)

// call with two parameters
foo(3, 4)

// call with named parameters
foo(b = 3)

// call with named and positional parameters
foo(3, b = 4)

// call with named and positional parameters
foo(
    a = 3, 
    b = 4
)

// call with named and positional parameters
foo(
    b = 3,
    a = 4
)