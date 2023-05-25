// Future features

// Typed arrays
mut arr: [..] int = [1, 2, 3]

// Typed Dicts
mut dict: { int : int } = { 1: 2, 3: 4 }

// Typed tuples
mut tup: (int, int) = (1, 2)

// Typed functions
add(a: int, b: int) -> int { a + b }

// Typed lambdas
mut lambda: (int, int) -> int

lambda = (a: int, b: int) -> int { a + b }

// Template functions
add[T](a: T, b: T) -> T { a + b }

// Template with constraints
add[T = int | float ](a: T, b: T) -> T { a + b }