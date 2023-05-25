
class Animal {
     mut name: str = "Animal"

    mInit(name: str) { 
        self.name = name
    }

    GetName() -> str { self.name }
}

export { Animal }