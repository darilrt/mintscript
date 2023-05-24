
class Person {
    mut name: str
    mut age: int
    
    mConstructor(name: str, age: int) {
        self.name = name
        self.age = age
    }

    SayHello() {
        print("Hello, my name is", self.name, "and I am", self.age, "years old.")
    }
}

let d: Person = Person()

d.name = "Dylan"
d.age = 21

d.SayHello()