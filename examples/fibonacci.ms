// Fibonacci in ZenScript

// This is a recursive function
fib (n: int) -> int {
    if n == 0 {
        ret 0
    } elif n == 1 {
        ret 1
    } else {
        ret fib(n - 1) + fib(n - 2)
    }
}

// This is a loop
fib2 (n: int) -> int {
    mut a : int = 0
    mut b : int = 1
    
    for i in 0..n {
        c = a + b
        a = b
        b = c
    }
    ret a
}