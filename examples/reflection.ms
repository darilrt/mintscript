
import zs.reflection.*

class Test {
    mut a : int

    Test (a : int) {
        .a = a
    }
}

main () {
    let t : Test = Test(10)

    // Get the type of t
    let t_type : Type = typeOf(t)

    // Get fields of t
    let fields : [..] Field = t_type.fields()
    
    // Print the name of each field
    for field in fields {
        print(field.name())
    }
    
    for i in 0..fields.Len() {
        print(fields[i].name())
    }

    // Output:
    // a

    // Get the type of the field a
    let a_type : Type = typeOf(t.a)
    
    // Print the name of the type of a
    print(a_type.name())
    
    // Output:
    // int

    // Get the value of a in t with reflection
    let a_value : int = t_type.fieldValue(t, "a")

    // Print the value of a
    print(a_value)

    // Output:
    // 10

    // Set the value of a in t with reflection
}