import gl
import sdl2

main () {
    sdl2.Init(sdl2.INIT_VIDEO)

    let window := sdl2.CreateWindow("Hello World", 100, 100, 640, 480, sdl2.OPENGL)
    let glContext := sdl2.GlCreateContext(window)

    mut event : sdl2.Event = sdl2.Event()
    mut run : bool = true
    
    while run {
        while sdl2.PollEvent(event) {
            if event.type == sdl2.QUIT {
                run = false
            }
        }

        gl.ClearColor(0.2, 0.3, 0.3, 1.0)
        gl.Clear(gl.COLOR_BUFFER_BIT)

        gl.Begin(gl.TRIANGLES)
        gl.Vertex3f(-0.5, -0.5, 0.0)
        gl.Vertex3f(0.5, -0.5, 0.0)
        gl.Vertex3f(0.0, 0.5, 0.0)
        gl.End()

        sdl2.GlSwapWindow(window)
    }

    sdl2.GlDeleteContext(glContext)
    sdl2.DestroyWindow(window)
    sdl2.Quit()
}