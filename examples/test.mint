import minty

mut window: Window = Window("Hello, Minty!", 600, 600)
mut event: Event = Event()
mut renderer: Renderer = Renderer(window)

mut x: int = 0
mut move: bool = false

mut running: bool = true
while running {
    delay(1)

    while event.Poll() {
        if event.IsQuit() {
            running = false
        }
        elif event.IsMouseButtonDown() {
            if event.button == 1 {
                move = true
            }
        }
        elif event.IsMouseButtonUp() {
            if event.button == 1 {
                move = false
            }
        }
    }

    if move {
        x = x + 1
    }

    renderer.SetDrawColor(0, 0, 0, 255)
    renderer.Clear()

    renderer.SetDrawColor(0, 255, 0, 255)
    renderer.DrawRect(x, 50, 100, 100)

    renderer.Present()
}