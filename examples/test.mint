
class Vec2 {
    mut x: float
    mut y: float

    Vec2(x: float, y: float) {
        this.x = x
        this.y = y
    }

    ToStr() -> str {
        return "Vec2(".Concat(this.x.ToStr()).Concat(", ").Concat(this.y.ToStr()).Concat(")")
    }
}

mut v: Vec2 = Vec2(1.0, 2.0)

print(v.ToStr())