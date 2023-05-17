// Title: Test

callMyFunction(f : Function) {
    print("returned", f())
}

a() -> int {
    print("Hello World!")
    10
}

callMyFunction(a) 
