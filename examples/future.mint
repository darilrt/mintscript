
// Ternary Operator
let a: int = if true { 1 } else { 2 }

// Match
let a: int = match 1 {
    1 -> { 1 }
    2 -> { 2 }
    _ -> { 3 }
}

// Type Inference
let a := 1

// Template Functions
add[T](a: T, b: T) -> T { a + b }

// Template Types
class Pair[T, U] {
    let first: T
    let second: U
}

// Template Constraints
class Pair[T: int | float, U: int | float] {
    let first: T
    let second: U
}

// Attributes
@[Serializable]
class Person {
    let name: str
    let age: int
}

// ORM
class Table(Attribute) {
    let name: str

    (name: str) { self.name = name }
}

class PrimaryKey(Attribute) { }

class AutoIncrement(Attribute) { }

class Column(Attribute) {
    let name: str

    (name: str) { self.name = name }
}

@[db.Table("people")]
class Person {
    @[db.PrimaryKey(), db.AutoIncrement(), db.Column("person_id")]
    let id: int

    @[db.Column("person_name")]
    let name: str

    @[db.Column("person_age")]
    let age: int
}

// Get all classes with the Table attribute
GetTables() -> List[Type] {
    reflect.GetTypes().Filter((t: Type) -> bool { t.HasAttribute(Table) })
}

// Get all columns for a class
GetColumns(t: Type) -> List[Field] {
    t.GetFields().Filter((f: Field) -> bool { f.HasAttribute(Column) })
}

// Generate SQL
GenerateSQL(t: Type) -> str {
    let columns: List[Field] = GetColumns(t)
    let sql: str = "CREATE TABLE " + t.GetAttribute(Table).name + " ("
    
    for (let i: int = 0; i < columns.Count(); i++) {
        let column: Field = columns[i]
        let columnType: str = column.Type().Name()
        let columnName: str = column.GetAttribute(Column).name
        sql += columnName + " " + columnType
        if (i < columns.Count() - 1) {
            sql += ", "
        }
    }
    sql += ")"

    sql
}

// Generate SQL for all tables
GenerateSQL() -> List[str] {
    let tables: List[Type] = GetTables()
    let sql: List[str] = tables.Map((t: Type) -> str { GenerateSQL(t) })
    sql
}

// Generate SQL for all tables and execute
GenerateSQLAndExecute() {
    let sql: List[str] = GenerateSQL()
    for (let i: int = 0; i < sql.Count(); i++) {
        let s: str = sql[i]
        db.Execute(s)
    }
}