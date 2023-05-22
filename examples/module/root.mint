
sum(list : List) -> int {
    mut result : int = 0

    mut i : int = 0
    let len : int = list.Length()
    while i < len {
        result = result + list[i]
        i = i + 1
    }

    ret result
}

export { sum }
