import gl
import sdl2

main () {
    sdl2.init(sdl2.INIT_VIDEO)

    let window := sdl2.createWindow("Hello World", 100, 100, 640, 480, sdl2.OPENGL)
    let glContext := sdl2.glCreateContext(window)

    mut event := sdl2.Event()
    mut run := true
    
    while run {
        while sdl2.pollEvent(event) {
            if event.type == sdl2.QUIT {
                run = false
            }
        }

        gl.clearColor(0.2, 0.3, 0.3, 1.0)
        gl.clear(gl.COLOR_BUFFER_BIT)

        gl.Begin(gl.TRIANGLES)
        gl.Vertex3f(-0.5, -0.5, 0.0)
        gl.Vertex3f(0.5, -0.5, 0.0)
        gl.Vertex3f(0.0, 0.5, 0.0)
        gl.End()

        sdl2.glSwapWindow(window)
    }

    sdl2.glDeleteContext(glContext)
    sdl2.destroyWindow(window)
    sdl2.quit()
}