// Built-In data types

// int - integer
mut x : int = 1

// long - long integer
mut y : long = 1

// float - floating point number
mut z : float = 1.0

// double - double precision floating point number
mut a : double = 1.0

// str - string
mut b : str = "Hello World"

// bool - boolean
mut c : bool = true

// List - list
mut d : [..] int = [1, 2, 3]

// Array - array
mut d : [3] int = [1, 2, 3]

// Tuple - tuple
mut e : (int, str) = (1, "Hello World")

// Set - set
mut f : Set<int> = [1, 2, 3]

// Dict - dictionary
mut g : [str : str] = { "key": "value" } 

// any - any type
mut h : any = 1
mut i : any = "Hello World"
mut j : [str : any] = { "key": 1, "key2": "Hello World" }

class Dict<K, V> {
    Keys () -> [..] K { ret [] }
}