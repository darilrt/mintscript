
class Animal {
    mut name: str

    SayHello() {
        print(this.name)
    }

}

Hello(b: Animal) {
    print(b.name)
}

let a: Animal = Animal()

a.name = "Dog"

Hello(a)