#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cafe.h"
#include "tea.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#define PROJECTION_UNIFORM_NAME "u_World"
#define MODELVIEW_UNIFORM_NAME "u_ModelView" 

#define POSITION_ATTRIBUTE_NAME "a_Position"
#define COLOR_ATTRIBUTE_NAME "a_Color"
#define TEXCOORD_ATTRIBUTE_NAME "a_TexCoord"

#define MAX_DRAW_CALLS 1024

#define cafe() (&_cafe_ctx)
#define render() (&cafe()->render)
#define input() (&cafe()->input)

typedef SDL_Window ca_Window;
typedef SDL_Event ca_Event;

static const char *_150_vert_header =
"#version 150\n"
"uniform mat4 u_World;\n"
"uniform mat4 u_ModelView;\n"
"in vec3 a_Position;\n"
"in vec4 a_Color;\n"
"in vec2 a_TexCoord;\n"
"out vec4 v_Color;\n"
"out vec2 v_TexCoord;\n";

static const char *_150_frag_header =
"#version 150\n"
"in vec4 v_Color;\n"
"in vec2 v_TexCoord;\n"
"uniform sampler2D u_Texture;\n"
"out vec4 o_FragColor;\n";

static const char *_120_vert_header =
"#version 120\n"
"uniform mat4 u_World;\n"
"uniform mat4 u_ModelView;\n"
"attribute vec3 a_Position;\n"
"attribute vec4 a_Color;\n"
"attribute vec2 a_TexCoord;\n"
"varying vec4 v_Color;\n"
"varying vec2 v_TexCoord;\n";

static const char *_120_frag_header =
"#version 120\n"
"varying vec4 v_Color;\n"
"varying vec2 v_TexCoord;\n"
"uniform sampler2D u_Texture;\n"
"#define o_FragColor gl_FragColor\n"
"#define texture texture2D\n";

static const char *_default_vert_function =
"vec4 position(mat4 model_view, mat4 world, vec3 pos) {\n"
"  return model_view * world * vec4(pos, 1.0);\n"
"}\n";

static const char *_default_frag_function =
"vec4 pixel(vec4 color, vec2 tex_coord, sampler2D tex) {\n"
"  return color * texture(tex, tex_coord);\n"
"}\n";

static const char *_vert_main =
"void main() {\n"
"  v_Color = a_Color;\n"
"  v_TexCoord = a_TexCoord;\n"
"  gl_Position = position(u_ModelView, u_World, a_Position);\n"
"}\n";

static const char *_frag_main =
"void main() {\n"
"  o_FragColor = pixel(v_Color, v_TexCoord, u_Texture);\n"
"}\n";

struct ca_Shader {
    te_shader_t handle;
    ca_i32 world_loc;
    ca_i32 modelview_loc;
    ca_i32 texture_loc;
};

struct ca_Texture {
    te_texture_t handle;
};

struct ca_Batch {
    ca_Texture *texture;
    te_vao_t *vao;
    te_buffer_t *vbo, *ibo;

    ca_i32 start_index, num_vertices;
};

typedef struct ca_DrawCall ca_DrawCall;
struct ca_DrawCall {
    ca_i32 mode;
    ca_Color clear_color;
    ca_Shader *shader;
    ca_Camera *camera;
    ca_Texture *target;
    ca_Batch *batch;
};

struct ca_Render {
    struct {
        ca_Texture *white_texture;
        ca_Texture *target_texture;
        ca_Shader *shader;
    } defaults;

    ca_DrawCall *draw_calls;
    ca_i32 max_draw_calls;
    ca_i32 draw_call_index;

    ca_DrawCall *current_draw_call;

    ca_DrawCall draw_call;
};

struct ca_Input {
    const Uint8 *keys;
    struct {
	Uint8 down[3];
	Uint8 pressed[3];
    } mouse;
};

struct Cafe {
    ca_Window *window;
    SDL_GLContext context;
    ca_Event event;
    struct ca_Render render;
    struct ca_Input input;
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
                                      SDL_WINDOWPOS_CENTERED,
                                      SDL_WINDOWPOS_CENTERED,
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
    cafe()->render.defaults.shader = cafe_render_createShader(NULL, NULL);
    render()->draw_calls = malloc(sizeof(ca_DrawCall) * MAX_DRAW_CALLS);
    render()->max_draw_calls = MAX_DRAW_CALLS;
    render()->draw_call_index = 0;
    render()->current_draw_call = render()->draw_calls;
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
    ca_i32 width, height;
    SDL_GetWindowSize(cafe()->window, &width, &height);
    tea_viewport(0, 0, width, height);
    tea_matrix_mode(TEA_PROJECTION);
    tea_load_identity();
    tea_ortho(0, width, height, 0, -1, 1);
    tea_matrix_mode(TEA_MODELVIEW);
    tea_load_identity();

    // ca_Shader *shader = cafe()->render.state.shader;
    ca_Shader *shader = render()->current_draw_call->shader;
    tea_use_shader(shader->handle);
    tea_uniform_matrix4fv(shader->world_loc, 1, CA_FALSE, tea_get_matrix(TEA_PROJECTION));
    tea_uniform_matrix4fv(shader->modelview_loc, 1, CA_FALSE, tea_get_matrix(TEA_MODELVIEW));
    tea_begin(TEA_TRIANGLES);
}
void cafe_end() {
    ca_Shader *shader = render()->draw_call.shader;
    tea_use_shader(shader->handle);
    tea_uniform_matrix4fv(shader->world_loc, 1, CA_FALSE, tea_get_matrix(TEA_PROJECTION));
    tea_uniform_matrix4fv(shader->modelview_loc, 1, CA_FALSE, tea_get_matrix(TEA_MODELVIEW));
    tea_end();
    tea_use_shader(0);
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

/*=====================*
 * RENDER FUNCTIONS    *
 *=====================*/

void cafe_render_begin(void) {
    te_uint draw_mode = render()->draw_call.mode == CA_FILL ? TEA_TRIANGLES : TEA_LINES;
    tea_begin(draw_mode);
}

void cafe_render_end(void) {
    tea_end();
}


ca_i32 cafe_render_mode(ca_i32 mode) {
    ca_i32 old_mode = cafe()->render.state.mode;
    cafe()->render.state.mode = mode;
    return old_mode;
}

void cafe_render_setShader(ca_Shader* shader) {
    cafe()->render.state.shader = shader ? shader : cafe()->render.defaults.shader;
}

/*****************
 * Shader       *
 ****************/

ca_Shader* cafe_render_createShader(const char* vert, const char* frag) {
    vert = vert ? vert : _default_vert_function;
    frag = frag ? frag : _default_frag_function;

    ca_Shader* shader = malloc(sizeof(ca_Shader));
    memset(shader, 0, sizeof(ca_Shader));
    const char *vert_shader_strs[] = { _120_vert_header, vert, _vert_main };
    const char *frag_shader_strs[] = { _120_frag_header, frag, _frag_main };

    int vert_len, frag_len;
    vert_len = frag_len = 0;
    for (int i = 0; i < 3; i++) {
        vert_len += strlen(vert_shader_strs[i]);
        frag_len += strlen(frag_shader_strs[i]);
    }

    char vert_source[vert_len + 1];
    char frag_source[frag_len + 1];
    vert_source[0] = frag_source[0] = '\0';
    for (int i = 0; i < 3; i++) {
        strcat(vert_source, vert_shader_strs[i]);
        strcat(frag_source, frag_shader_strs[i]);
    }
    vert_source[vert_len] = frag_source[frag_len] = '\0';

    shader->handle = tea_shader(vert_source, frag_source);
    shader->world_loc = tea_get_uniform_location(shader->handle, "u_World");
    shader->modelview_loc = tea_get_uniform_location(shader->handle, "u_ModelView");
    shader->texture_loc = tea_get_uniform_location(shader->handle, "u_Texture");
    return shader;
}

void cafe_shader_destroy(ca_Shader *shader) {
    if (!shader) return;
    tea_free_shader(shader->handle);
    free(shader);
}

void cafe_shader_setUniform1f(ca_Shader *shader, const char *name, ca_f32 value) {
    if (!shader)
	return;
    int loc = tea_get_uniform_location(shader->handle, name);
    te_shader_t current = cafe()->render.state.shader ? cafe()->render.state.shader->handle : 0;
    tea_use_shader(shader->handle);
    tea_uniform1f(loc, value);
    tea_use_shader(current);
}

void cafe_shader_setUniform2f(ca_Shader *shader, const char *name, ca_f32 v0, ca_f32 v1) {
    if (!shader)
	return;
    int loc = tea_get_uniform_location(shader->handle, name);
    te_shader_t current = render()->state.shader ? render()->state.shader->handle : 0;
    tea_use_shader(shader->handle);
    tea_uniform2f(loc, v0, v1);
    tea_use_shader(current);

}

void cafe_shader_setUniform3f(ca_Shader *shader, const char *name, ca_f32 v0, ca_f32 v1, ca_f32 v2) {
    if (!shader)
	return;
    int loc = tea_get_uniform_location(shader->handle, name);
    te_shader_t current = render()->state.shader ? render()->state.shader->handle : 0;
    tea_use_shader(shader->handle);
    tea_uniform3f(loc, v0, v1, v2);
    tea_use_shader(current);
}

void cafe_shader_setUniform4f(ca_Shader *shader, const char *name, ca_f32 v0, ca_f32 v1, ca_f32 v2, ca_f32 v3) {
    if (!shader)
	return;
    int loc = tea_get_uniform_location(shader->handle, name);
    te_shader_t current = render()->state.shader ? render()->state.shader->handle : 0;
    tea_use_shader(shader->handle);
    tea_uniform4f(loc, v0, v1, v2, v3);
    tea_use_shader(current);
}

