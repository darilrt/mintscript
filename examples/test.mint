
class Base {
    let name: str = "Base"

    printName() {
        print(self.name)
    }
}

class Base2 {
    let name2: str = "Base2"

    printName2() {
        print(self.name2)
    }
}

class Derived(Base, Base2) { }

let d: Derived = Derived()

print(d.name)
print(d.name2)
