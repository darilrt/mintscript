module shape

import zs.reflection
import zs.test { assert }

interface Shape {
    Area () -> f32
}

class Circle impl Shape {
    mut radius : f32 = 0
    
    Circle (radius : f32) { .radius = radius }

    Area () -> f32 { ret 3.14 * .radius * .radius }
}

class Square impl Shape {
    mut side : f32 = 0

    Square (side : f32) { .side = side }

    Area () -> f32 { ret .side * .side }
}

test () {
    print("Running shape.zs tests...")

    // null is a valid value for a nullable type
    let shapes : [..] Shape? = [Circle(1), Square(2), Circle(3), Square(4), null]

    mut total_area := 0.0

    for shape : Shape? in shapes {
        total_area += shape?.Area() // ? is the safe operator
    }
    
    assert(total_area == 50.24)
}

export [
    Shape,
    Circle,
    Square,
]