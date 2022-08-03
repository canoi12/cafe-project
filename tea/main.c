#include "tea.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <stdio.h>

int main(int argc, char **argv) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        return 1;
    }
    SDL_Window *window = SDL_CreateWindow("Tea", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (window == NULL) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        return 1;
    }
    SDL_GLContext context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, context);
    SDL_Event event;
    // te_config_t config = tea_config();
    tea_init(NULL);

    te_shader_t shader = tea_shader(NULL, NULL);
    te_batch_t batch = tea_batch(1000);
    tea_batch_fill_triangle(batch, (vec2){16, 16}, (vec2){0, 32}, (vec2){32, 32});

    while(event.type != SDL_QUIT) {
        SDL_PollEvent(&event);
        tea_begin();
        tea_clear(TEA_RGB(255, 0, 255));
        tea_set_shader(shader);
        tea_batch_draw(batch);
        tea_end();
        // tea_begin();
        // tea_color4f(1.f, 0.f, 1.f, 1.f);
        // tea_vertex2f(32, 32);
        // tea_vertex2f(0, 0);
        // tea_vertex2f(0, 32);
        // tea_vertex2f(0, 0);
        // tea_vertex2f(32, 32);
        // tea_vertex2f(32, 0);
        // tea_end();
        SDL_GL_SwapWindow(window);
    }

    tea_quit();
    return 0;
}
