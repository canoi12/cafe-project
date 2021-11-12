#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cafe.h"
#include "tea.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#define cafe() (&_cafe_ctx)

typedef SDL_Window ca_Window;
typedef SDL_Event ca_Event;

const char *vert_shader =
"#version 120\n"
"uniform mat4 u_World;\n"
"uniform mat4 u_ModelView;\n"
"attribute vec4 a_Position;\n"
"attribute vec4 a_Color;\n"
"varying vec4 v_Color;\n"
"void main() {\n"
"  v_Color = a_Color;\n"
"  gl_Position = u_ModelView * u_World * a_Position;\n"
"}\n";

const char *frag_shader =
"#version 120\n"
"varying vec4 v_Color;\n"
"void main() {\n"
"  gl_FragColor = v_Color;\n"
"}\n";


struct ca_Texture {
    te_texture_t handle;
};

struct ca_Render {
    struct {} defaults;
    struct {} state;
};

struct ca_Input {
    const Uint8 *keys;
};

struct Cafe {
    ca_Window *window;
    SDL_GLContext context;
    ca_Event event;
    struct ca_Render render;

    struct {
        const Uint8 *keys;
        struct {
            ca_bool down[3];
            ca_bool pressed[3];
        } mouse;
    } input;
};

static Cafe _cafe_ctx;

ca_Config cafe_config(const char* title, ca_i32 width, ca_i32 height)
{
    ca_Config config;
    memset(&config, 0, sizeof(ca_Config));
    const char *t = title ? title : "cafe";
    memcpy(config.window.title, t, strlen(t));
    config.window.width = width;
    config.window.height = height;
    config.window.flags = SDL_WINDOW_OPENGL;
    return config;
}

ca_bool cafe_open(ca_Config* config) {
    memset(cafe(), 0, sizeof(Cafe));
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return CA_FALSE;
    }

    cafe()->window = SDL_CreateWindow(config->window.title,
                                      SDL_WINDOWPOS_UNDEFINED,
                                      SDL_WINDOWPOS_UNDEFINED,
                                      config->window.width,
                                      config->window.height,
                                      config->window.flags);
    if (cafe()->window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return CA_FALSE;
    }

    cafe()->context = SDL_GL_CreateContext(cafe()->window);
    if (cafe()->context == NULL) {
        printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
        return CA_FALSE;
    }
    SDL_GL_MakeCurrent(cafe()->window, cafe()->context);
    te_config_t tea_conf = tea_config();
    tea_init(&tea_conf);
    cafe()->input.keys = SDL_GetKeyboardState(NULL);
    return CA_TRUE;
}

void cafe_close(void) {
    tea_quit();
}

ca_bool cafe_is_running(void) {
    return !SDL_QuitRequested();
}

void cafe_begin() {
    while (SDL_PollEvent(&cafe()->event)) {
        if (cafe()->event.type == SDL_QUIT)
            return;

        if (cafe()->event.type == SDL_MOUSEBUTTONDOWN) {
            cafe()->input.mouse.down[cafe()->event.button.button] = CA_TRUE;
        }
        if (cafe()->event.type == SDL_MOUSEBUTTONUP) {
            cafe()->input.mouse.down[cafe()->event.button.button] = CA_FALSE;
        }
    }
    tea_clear_color(.3f, .4f, .4f, 1.f);
    tea_clear();
}
void cafe_end() {
    SDL_GL_SwapWindow(cafe()->window);
}

void cafe_run(ca_UpdateFunction update) {
    while (cafe_is_running())  {
        cafe_begin();
        if (update) update(1.f);
        cafe_end();
    }
}

/*=====================*
 * INPUT FUNCTIONS      *
 *=====================*/

/*****************
 * Keyboard      *
 ****************/

ca_i32 cafe_keyboard_fromName(const char* name) {
    return SDL_GetScancodeFromName(name);
}

const char* cafe_keyboard_getName(ca_i32 key) {
    return SDL_GetScancodeName(key);
}

ca_bool cafe_keyboard_isDown(ca_i32 key) {
    return cafe()->input.keys[key];
}

ca_bool cafe_keyboard_isUp(ca_i32 key) {
    return !cafe()->input.keys[key];
}

ca_bool cafe_keyboard_wasPressed(ca_i32 key);
ca_bool cafe_keyboard_wasReleased(ca_i32 key);

/*****************
 * Mouse         *
 ****************/

void cafe_mouse_getPos(ca_f32* x, ca_f32* y) {
    int xx, yy;
    SDL_GetMouseState(&xx, &yy);
    if (x) *x = xx;
    if (y) *y = yy;
}

void cafe_mouse_setPos(ca_f32 x, ca_f32 y) {
    SDL_WarpMouseInWindow(cafe()->window, x, y);
}

void cafe_mouse_getDeltaPos(ca_f32* x, ca_f32* y) {
    int xx, yy;
    SDL_GetRelativeMouseState(&xx, &yy);
    if (x) *x = xx;
    if (y) *y = yy;
}

ca_bool cafe_mouse_isDown(ca_i32 button) {
    return SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(button);
}

ca_bool cafe_mouse_isUp(ca_i32 button) {
    return !(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(button));
}

ca_bool cafe_mouse_wasPressed(ca_i32 button) {
    return CA_TRUE;
}

ca_bool cafe_mouse_wasReleased(ca_i32 button) {
    return CA_TRUE;
}
