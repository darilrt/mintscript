
class Vec2 {
    mut x: float = 0.0
    mut y: float = 0.0

    mInit(x: float, y: float) {
        self.x = x
        self.y = y
    }

    ToString() -> str { "Vec2(" + self.x + ", " + self.y + ")" }
}

export { Vec2 }