# Tea

OpenGL loader and simple immediate mode core versions

### using SDL2
```c
#include <stdio.h>
#include "tea.h"
#include <SDL2/SDL.h>

const char *vert =
"#version 120\n"
"attribute vec3 a_Position;\n"
"attribute vec4 a_Color;\n"
"varying vec4 v_Color;\n"
"void main() {\n"
"  gl_Position = vec4(a_Position, 1.0);\n"
"  v_Color = a_Color;\n"
"}\n";

const char *frag =
"#version 120\n"
"varying vec4 v_Color;\n"
"void main() {\n"
"  gl_FragColor = v_Color;\n"
"}\n";

int main(int argc, char **argv) {
    SDL_Window *window;
    SDL_GLContext context;
    SDL_Event event;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

    window = SDL_CreateWindow("SDL2 OpenGL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_OPENGL);
    if (window == NULL) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    context = SDL_GL_CreateContext(window);
    if (context == NULL) {
        printf("SDL_GL_CreateContext Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    SDL_GL_MakeCurrent(window, context);
    SDL_GL_SetSwapInterval(1);

    te_config_t config = tea_config();
    tea_init(&config);

    te_shader_t shader = tea_shader(vert, frag);
    while (event.type != SDL_QUIT) {
        SDL_PollEvent(&event);
        tea_clear_color(0.3f, 0.4f, 0.4f, 1.0f);
        tea_clear();

        tea_use_shader(shader);
        tea_begin(TEA_TRIANGLES);
        tea_color3f(1.f, 0.f, 0.f);
        tea_vertex2f(-0.5f, -0.5f);
        tea_color3f(0.f, 1.f, 0.f);
        tea_vertex2f(0.5f, -0.5f);
        tea_color3f(0.f, 0.f, 1.f);
        tea_vertex2f(0.f, 0.5f);
        tea_end();
        tea_use_shader(0);
        SDL_GL_SwapWindow(window);
    }

    tea_quit();
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
```

### using GLFW
```c
#include <stdio.h>
#include "tea.h"
#include <GLFW/glfw3.h>

const char *vert =
"#version 120\n"
"attribute vec3 a_Position;\n"
"attribute vec4 a_Color;\n"
"varying vec4 v_Color;\n"
"void main() {\n"
"  gl_Position = vec4(a_Position, 1.0);\n"
"  v_Color = a_Color;\n"
"}\n";

const char *frag =
"#version 120\n"
"varying vec4 v_Color;\n"
"void main() {\n"
"  gl_FragColor = v_Color;\n"
"}\n";

int main(int argc, char **argv) {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    GLFWwindow *window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to open GLFW window.\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    te_config_t config = tea_config();
    tea_init(&config);

    te_shader_t shader = tea_shader(vert, frag);

    while (!glfwWindowShouldClose(window)) {
        tea_clear_color(0.3f, 0.4f, 0.4f, 1.0f);
        tea_clear();

        tea_use_shader(shader);
        tea_begin(TEA_TRIANGLES);
        tea_color3f(1.f, 0.f, 0.f);
        tea_vertex2f(-0.5f, -0.5f);
        tea_color3f(0.f, 1.f, 0.f);
        tea_vertex2f(0.5f, -0.5f);
        tea_color3f(0.f, 0.f, 1.f);
        tea_vertex2f(0.f, 0.5f);
        tea_end();
        tea_use_shader(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    tea_quit();
    glfwTerminate();
    return 0;
}
```
