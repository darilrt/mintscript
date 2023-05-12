class Error : Exception {
    let message : str

    Init (message : str) {
        .message = message
    }

    ToString () -> str {
        return "Error: " + .message
    }
}

main () -> void {
    raise("This is an error")

    try {
        raise("This is an error")
    } catch e : Error {
        print(e)
    }

    // Custom error
    let e : Error = Error("This is a custom error")
    e.Raise()
}