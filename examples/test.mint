
class Animal {
    mut name: str

    Animal(name: str) {
        this.name = name
    }

    Speak() {
        print(this.name)
    }
}

let a: Animal = Animal("Hello")

a.Speak()
