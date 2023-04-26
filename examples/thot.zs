
// Consulta de un paciente
class Consulta {
    mut id : int = 0
    mut paciente : string = ""
    mut fecha : string = ""
}

// terminal para gestionar las consultas de un consultorio de dentista
class Terminal {
    mut menu : string = "main"
    mut consuts : [..] 

    Run () {
        mut isRunning : bool = true

        while isRunning {
            if .menu == "main" {
                .MainMenu()
            } elif .menu == "new" {
                .NewConsulta()
            } elif .menu == "list" {
                .ListConsultas()
            } elif .menu == "exit" {
                isRunning = false
            }
        }
    }

    MainMenu () {
        print("1. Nueva consulta")
        print("2. Listar consultas")
        print("3. Salir")

        mut option : int = input()

        if option == 1 {
            .menu = "new"
        } elif option == 2 {
            .menu = "list"
        } elif option == 3 {
            .menu = "exit"
        }
    }

    NewConsulta () {
        mut consulta : Consulta = new Consulta()

        print("Ingrese el nombre del paciente")
        consulta.paciente = input()

        print("Ingrese la fecha de la consulta")
        consulta.fecha = input()

        .consuts.Push(consulta)

        .menu = "main"
    }

    ListConsultas () {
        for consult : Consulta in .consuts {
            print("Paciente: " + consult.paciente)
            print("Fecha: " + consult.fecha)
            print("")
        }

        .menu = "main"
    }
}

main () {
    let term : Terminal = new Terminal()
    
    term.Run()
}