fibonacci(n : int) -> int {
    if n < 2 { return n }
    return fibonacci(n - 1) + fibonacci(n - 2)
}

print(fibonacci(9))
