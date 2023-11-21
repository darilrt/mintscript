#include "MintScript.h"

#include <SDL.h>
#include <chrono>
#include <ctime>

namespace minty {
    class Vector2I : public ir::Object {
    public:
        Vector2I(int x, int y) : ir::Object(2) {
            fields[0] = { ir::Mainfold::Int, x };
            fields[1] = { ir::Mainfold::Int, y };
        }
    };

    class Vector2 : public ir::Object {
    public:
        Vector2(float x, float y) : ir::Object(2) {
            fields[0] = { ir::Mainfold::Float, x };
            fields[1] = { ir::Mainfold::Float, y };
        }

        std::string ToStr() {
            return "Vec2(" + std::to_string(fields[0].value.f) + ", " + std::to_string(fields[1].value.f) + ")";
        }
    };

    class Window : public ir::Object {
    public:
        SDL_Window* sdlWindow;

        Window(const std::string& title, int width, int height) : ir::Object(0) {
            sdlWindow = SDL_CreateWindow(
                title.c_str(),
                SDL_WINDOWPOS_UNDEFINED,
                SDL_WINDOWPOS_UNDEFINED,
                width,
                height,
                SDL_WINDOW_SHOWN
            );
        }
    };

    class Event : public ir::Object {
    public:
        Event() : ir::Object(2) { 
            fields[0] = { ir::Mainfold::Int, 0 };
            fields[1] = { ir::Mainfold::Int, 0 };
        }

        bool Poll() {
            return SDL_PollEvent(&sdlEvent);
        }

        bool IsQuit() {
            return sdlEvent.type == SDL_QUIT;
        }

        bool IsKeyDown() {
            const bool isKeyDown = sdlEvent.type == SDL_KEYDOWN;

            if (isKeyDown) {
                fields[0].value.i = sdlEvent.key.keysym.scancode;
            }

            return isKeyDown;
        }

        bool IsKeyUp() {
            const bool isKeyUp = sdlEvent.type == SDL_KEYUP;

            if (isKeyUp) {
                fields[0].value.i = sdlEvent.key.keysym.scancode;
            }

            return isKeyUp;
        }

        bool IsMouseButtonDown() {
            const bool mouseButtonDown = sdlEvent.type == SDL_MOUSEBUTTONDOWN;

            if (mouseButtonDown) {
                fields[1].value.i = sdlEvent.button.button;
            }

            return mouseButtonDown;
        }

        bool IsMouseButtonUp() {
            const bool mouseButtonUp = sdlEvent.type == SDL_MOUSEBUTTONUP;

            if (mouseButtonUp) {
                fields[1].value.i = sdlEvent.button.button;
            }

            return mouseButtonUp;
        }

    private:
        SDL_Event sdlEvent;
    };

    class Renderer : public ir::Object {
    public:
        Renderer(Window* window) : ir::Object(0) {
            sdlRenderer = SDL_CreateRenderer(window->sdlWindow, -1, SDL_RENDERER_ACCELERATED);
        }

        void SetDrawColor(int r, int g, int b, int a) {
            SDL_SetRenderDrawColor(sdlRenderer, r, g, b, a);
        }

        void DrawRect(int x, int y, int w, int h) {
            SDL_Rect rect = { x, y, w, h };
            SDL_RenderFillRect(sdlRenderer, &rect);
        }

        void Clear() {
            SDL_RenderClear(sdlRenderer);
        }

        void Present() {
            SDL_RenderPresent(sdlRenderer);
        }

    private:
        SDL_Renderer* sdlRenderer;
    };
}

ir::Mainfold delay(std::vector<ir::Mainfold> args) {
    SDL_Delay((Uint32)args[0].value.i);
    return { ir::Mainfold::Null };
}

ir::Mainfold getMousePosition(std::vector<ir::Mainfold> args) {
    int x, y;
    SDL_GetMouseState(&x, &y);
    return { ir::Mainfold::Object, (ir::Object*)new minty::Vector2(x, y) };
}

extern "C" void mint_Root() {
    mint::Function("delay", { t_void, t_int }, delay);

    auto t_vec2i = mint::Type("Vector2I", {
        { "x", t_int },
        { "y", t_int },
    }, {
        { "Vector2I", { nullptr, t_int, t_int }, [](std::vector<ir::Mainfold> args) -> ir::Mainfold {
            minty::Vector2I* vec2 = new minty::Vector2I(
                args[1].value.i,
                args[2].value.i
            );
            return { ir::Mainfold::Object, (ir::Object*)vec2 };
        }},
    });

    auto t_vec2 = mint::Type("Vector2", {
        { "x", t_float },
        { "y", t_float },
    }, {
        { "Vector2", { nullptr, t_float, t_float }, [](std::vector<ir::Mainfold> args) -> ir::Mainfold {
            minty::Vector2* vec2 = new minty::Vector2(
                args[1].value.f,
                args[2].value.f
            );
            return { ir::Mainfold::Object, (ir::Object*)vec2 };
        }},
        { "ToVector2I", { t_vec2i }, [](std::vector<ir::Mainfold> args) -> ir::Mainfold {
            minty::Vector2* vec2 = (minty::Vector2*)args[0].value.st;
            return { ir::Mainfold::Object, (ir::Object*)new minty::Vector2I(
                (int)vec2->fields[0].value.f,
                (int)vec2->fields[1].value.f
            )};
        }},
        { "ToStr", { t_str }, [](std::vector<ir::Mainfold> args) -> ir::Mainfold {
            minty::Vector2* vec2 = (minty::Vector2*)args[0].value.st;
            return { ir::Mainfold::String, new std::string(vec2->ToStr()) };
        }},
    });

    auto t_window = mint::Type("Window", { }, {
        { "Window", { nullptr, t_str, t_int, t_int }, [](std::vector<ir::Mainfold> args) -> ir::Mainfold {
            minty::Window* window = new minty::Window(
                *args[1].value.s,
                args[2].value.i,
                args[3].value.i
            );
            return { ir::Mainfold::Object, (ir::Object*)window };
        }},
    });

    mint::Type("Event", { 
        { "key", t_int },
        { "button", t_int },
    }, {
        { "Event", { nullptr }, [](std::vector<ir::Mainfold> args) -> ir::Mainfold {
            minty::Event* event = new minty::Event();
            return { ir::Mainfold::Object, (ir::Object*)event };
        }},
        { "Poll", { t_bool }, [](std::vector<ir::Mainfold> args) -> ir::Mainfold {
            minty::Event* event = (minty::Event*)args[0].value.st;
            return { ir::Mainfold::Bool, event->Poll() };
        }},
        { "IsQuit", { t_bool }, [](std::vector<ir::Mainfold> args) -> ir::Mainfold {
            minty::Event* event = (minty::Event*)args[0].value.st;
            return { ir::Mainfold::Bool, event->IsQuit() };
        }},
        { "IsKeyDown", { t_bool }, [](std::vector<ir::Mainfold> args) -> ir::Mainfold {
            minty::Event* event = (minty::Event*)args[0].value.st;
            return { ir::Mainfold::Bool, event->IsKeyDown() };
        }},
        { "IsKeyUp", { t_bool }, [](std::vector<ir::Mainfold> args) -> ir::Mainfold {
            minty::Event* event = (minty::Event*)args[0].value.st;
            return { ir::Mainfold::Bool, event->IsKeyUp() };
        }},
        { "IsMouseButtonDown", { t_bool }, [](std::vector<ir::Mainfold> args) -> ir::Mainfold {
            minty::Event* event = (minty::Event*)args[0].value.st;
            return { ir::Mainfold::Bool, event->IsMouseButtonDown() };
        }},
        { "IsMouseButtonUp", { t_bool }, [](std::vector<ir::Mainfold> args) -> ir::Mainfold {
            minty::Event* event = (minty::Event*)args[0].value.st;
            return { ir::Mainfold::Bool, event->IsMouseButtonUp() };
        }},
    });

    mint::Type("Renderer", { }, {
        { "Renderer", { nullptr, t_window }, [](std::vector<ir::Mainfold> args) -> ir::Mainfold {
            minty::Renderer* renderer = new minty::Renderer((minty::Window*)args[1].value.st);
            return { ir::Mainfold::Object, (ir::Object*)renderer };
        }},
        { "SetDrawColor", { t_void, t_int, t_int, t_int, t_int }, [](std::vector<ir::Mainfold> args) -> ir::Mainfold {
            minty::Renderer* renderer = (minty::Renderer*)args[0].value.st;
            renderer->SetDrawColor(
                args[1].value.i,
                args[2].value.i,
                args[3].value.i,
                args[4].value.i
            );
            return { ir::Mainfold::Null };
        }},
        { "DrawRect", { t_void, t_int, t_int, t_int, t_int }, [](std::vector<ir::Mainfold> args) -> ir::Mainfold {
            minty::Renderer* renderer = (minty::Renderer*)args[0].value.st;
            renderer->DrawRect(
                args[1].value.i,
                args[2].value.i,
                args[3].value.i,
                args[4].value.i
            );
            return { ir::Mainfold::Null };
        }},
        { "Clear", { t_void }, [](std::vector<ir::Mainfold> args) -> ir::Mainfold {
            minty::Renderer* renderer = (minty::Renderer*)args[0].value.st;
            renderer->Clear();
            return { ir::Mainfold::Null };
        }},
        { "Present", { t_void }, [](std::vector<ir::Mainfold> args) -> ir::Mainfold {
            minty::Renderer* renderer = (minty::Renderer*)args[0].value.st;
            renderer->Present();
            return { ir::Mainfold::Null };
        }}
    });
}