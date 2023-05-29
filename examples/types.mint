// Future features

// Typed arrays
mut arr: [..] int = [1, 2, 3]

// Typed Dicts
mut dict: Dict[str, int] = { "a": 1, "b": 2 }

// Typed tuples
mut tup: Tuple[int, int] = (1, 2)

// Typed functions
add(a: int, b: int) -> int { a + b }

// Typed lambdas
mut lambda: (int, int) -> int

lambda = (a: int, b: int) -> int { a + b }

// Template functions
add[T](a: T, b: T) -> T { a + b }
