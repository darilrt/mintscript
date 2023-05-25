
class Animal {
    mut name: str = "Unknown"

    Speak() {
        print("I am an animal named", self.name, ".")
    }
}

let a: Animal = Animal()

a.Speak()