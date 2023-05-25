// Fibonacci in MintScript

fibonacci(n : int) -> int {
    if n < 2 { n }
    fibonacci(n - 1) + fibonacci(n - 2)
}

print(fibonacci(10))
