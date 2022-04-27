#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cafe.h"
#include "tea.h"
#include "mocha.h"
#include "latte.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "langs/lua_cafe.h"

#include "external/stb_image.h"

#define PROJECTION_UNIFORM_NAME "u_World"
#define MODELVIEW_UNIFORM_NAME "u_ModelView" 

#define POSITION_ATTRIBUTE_NAME "a_Position"
#define COLOR_ATTRIBUTE_NAME "a_Color"
#define TEXCOORD_ATTRIBUTE_NAME "a_TexCoord"

#define MAX_DRAW_CALLS 1024
#define cafe() (&_cafe_ctx)
#define render() (&cafe()->render)
#define input() (&cafe()->input)

#define state() (&render()->state.pool[render()->state.index])
#define node() (&render()->node.pool[state()->node_end])

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

typedef struct ca_RenderNode ca_RenderNode;
struct ca_RenderNode {
    ca_Texture *texture;
    ca_i32 draw_mode;
    te_vertex_t *vertex;
    ca_i32 index, count;
}; 

typedef struct ca_RenderState ca_RenderState;
struct ca_RenderState {
    ca_Shader *shader;
    ca_Texture *target;
    ca_bool clear_needed;
    ca_Color clear_color;
    ca_i32 node_start, node_end;
};

struct ca_Shader {
    te_program_t handle;
    ca_i32 world_loc;
    ca_i32 modelview_loc;
    ca_i32 texture_loc;
};

struct ca_Texture {
    ca_i8 usage;
    te_texture_t handle;
    te_fbo_t fbo;
    ca_i32 width, height;
    ca_i32 filter[2];
    ca_i32 wrap[2];
};

struct ca_Batch {
    ca_Texture *texture;
    te_vao_t *vao;
    te_buffer_t *vbo, *ibo;

    ca_i32 start_index, num_vertices;
};

struct ca_DrawCall {
    ca_i32 mode;
    ca_Color clear_color;
    ca_Shader *shader;
    ca_Camera *camera;
    ca_Texture *target;
    ca_Texture *texture;
    ca_Batch *batch;
};

struct ca_Render {
    struct {
        ca_Texture *white_texture;
        ca_Texture *target_texture;
        ca_Shader *shader;
    } defaults;

    // struct {
    //     ca_i8 mode;
    //     ca_Shader *shader;
    // } state;

    struct {
        ca_RenderState pool[32];
        ca_i32 index;
    } state;

    struct {
        ca_RenderNode pool[1024];
        ca_i32 index;
    } node;

    ca_DrawCall *draw_calls;
    ca_i32 max_draw_calls;
    ca_i32 draw_call_index;

    ca_DrawCall *current_draw_call;

    ca_DrawCall draw_call;
    te_vertex_t *vertex;

};

struct ca_Input {
    const Uint8 *keys;
    struct {
	Uint8 down[3];
	Uint8 pressed[3];
    } mouse;
};

struct Cafe {
    struct {
        ca_Window *handle;
        ca_i32 width, height;
    } window;
    SDL_GLContext context;
    ca_Event event;
    struct ca_Render render;
    struct ca_Input input;
};

static Cafe _cafe_ctx;
static ca_Texture s_white_texture;
static ca_Texture s_default_target;
static ca_Shader* s_default_shader;
static te_vao_t s_default_vao;

static ca_bool s_cafe_render_init(void);
static void s_cafe_render_draw(void);

ca_Config cafe_config(const char* title, ca_i32 width, ca_i32 height)
{
    ca_Config config;
    memset(&config, 0, sizeof(ca_Config));
    const char *t = title ? title : "cafe";
    memcpy(config.window.title, t, strlen(t));
    config.window.width = width;
    config.window.height = height;
    config.window.flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;
    return config;
}

ca_bool cafe_open(ca_Config* config) {
    memset(cafe(), 0, sizeof(Cafe));
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return CA_FALSE;
    }

    cafe()->window.handle = SDL_CreateWindow(config->window.title,
                                      SDL_WINDOWPOS_CENTERED,
                                      SDL_WINDOWPOS_CENTERED,
                                      config->window.width,
                                      config->window.height,
                                      config->window.flags);

    ca_Window *window = cafe()->window.handle;
    cafe()->window.width = config->window.width;
    cafe()->window.height = config->window.height;
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return CA_FALSE;
    }

    cafe()->context = SDL_GL_CreateContext(window);
    if (cafe()->context == NULL) {
        printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
        return CA_FALSE;
    }
    SDL_GL_MakeCurrent(window, cafe()->context);
    te_config_t tea_conf = tea_config();
    tea_init(&tea_conf);
    la_init(".");

    cafe()->input.keys = SDL_GetKeyboardState(NULL);
    // cafe()->render.defaults.shader = cafe_render_createShader(NULL, NULL);
    // render()->draw_calls = malloc(sizeof(ca_DrawCall) * MAX_DRAW_CALLS);
    // render()->max_draw_calls = MAX_DRAW_CALLS;
    // render()->draw_call_index = 0;
    // render()->current_draw_call = render()->draw_calls;

    if (!s_cafe_render_init()) {
        return CA_FALSE;
    }

    if (!cafe_lua_setup()) {
        return CA_FALSE;
    }

    return CA_TRUE;
}

void cafe_close(void) {
    tea_quit();
}

ca_bool cafe_is_running(void) {
    return !SDL_QuitRequested();
}

static int counter;
void cafe_begin() {
    while (SDL_PollEvent(&cafe()->event)) {
        if (cafe()->event.type == SDL_QUIT)
            return;

        switch (cafe()->event.type) {
            case SDL_MOUSEBUTTONDOWN:
                cafe()->input.mouse.down[cafe()->event.button.button] = CA_TRUE;
                break;
            case SDL_MOUSEBUTTONUP:
                cafe()->input.mouse.down[cafe()->event.button.button] = CA_FALSE;
                break;
            case SDL_WINDOWEVENT: {
                switch (cafe()->event.window.event) {
                    case SDL_WINDOWEVENT_RESIZED: {
                        cafe()->window.width = cafe()->event.window.data1;
                        cafe()->window.height = cafe()->event.window.data2;
                    }
                    break;
                }
            }
            break;
        }
    }

    cafe_render_begin();
    cafe_render_setColor(CAFE_RGB(255, 255, 255));
    tea_clear_color(0, 0, 0, 1.f);
    tea_clear();
    counter = 0;
}

void cafe_end() {
    cafe_render_end();
    // s_cafe_render_draw();
    //printf("drawcalls: %d\n", counter);
    SDL_GL_SwapWindow(cafe()->window.handle);
}

static void s_lua_load_function(void) {
     lua_getglobal(L, "cafe");
    if (!lua_isnil(L, -1)) {
        lua_getfield(L, -1, "_load");
        if (!lua_isnil(L, -1)) {
            int err = lua_pcall(L, 0, 0, 0);
            if (err) {
                const char *str = lua_tostring(L, -1);
            }
        }
        lua_pop(L, 1);
        lua_settop(L, 1);
    }
}

void cafe_run(ca_StepFunction step) {
    s_lua_load_function();
    step = step ? step : cafe_lua_step_function();
    while (cafe_is_running())  {
        cafe_begin();
        step(1.f);
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
    SDL_WarpMouseInWindow(cafe()->window.handle, x, y);
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

static gl_modes[] = {
    [CAFE_LINE] = TEA_LINES,
    [CAFE_FILL] = TEA_TRIANGLES
};

ca_bool s_cafe_render_init(void) {
    render()->vertex = tea_vertex(NULL, 10000);
    s_default_vao = tea_vao();
    tea_bind_vao(s_default_vao);
    tea_vao_attach_vertex(render()->vertex);
    tea_bind_vao(0);

    s_default_shader = cafe_render_createShader(NULL, NULL);

    s_white_texture.handle = tea_white_texture();
    s_white_texture.width = 1;
    s_white_texture.height = 1;
    s_white_texture.fbo = 0;

    ca_Texture *target = &s_default_target;
    target->handle = 0;
    target->usage = CAFE_TEXTURE_TARGET;
    target->width = cafe()->window.width;
    target->height = cafe()->window.height;
    target->fbo = 0;

    render()->state.index = 0;
    memset(state(), 0, sizeof(ca_RenderState));
    
    state()->shader = s_default_shader;
    state()->target = &s_default_target;

    memset(node(), 0, sizeof(ca_RenderNode));
    node()->texture = &s_white_texture;
    node()->vertex = render()->vertex;
    node()->draw_mode = CAFE_LINE;

    return CA_TRUE;
}

void s_cafe_render_state(ca_RenderState *state) {
    if (tea_vertex_is_empty(render()->vertex)) return;
    if (state->node_end - state->node_start == 0 &&
        node()->count == 0)
        return;
    counter++;
    ca_Shader *shader = state->shader;
    ca_Texture *target = state->target;

    ca_i32 width, height;
    width = target->width;
    height = target->height;

    tea_use_program(shader->handle);
    tea_matrix_mode(TEA_PROJECTION);
    tea_load_identity();
    tea_ortho(0, width, height, 0, -1, 1);

    tea_bind_fbo(TEA_FRAMEBUFFER, target->fbo);
    tea_viewport(0, 0, width, height);


    ca_Color *cl_col = &state->clear_color;

    if (state->clear_needed) {
        tea_clear_color(cl_col->r / 255.f, cl_col->g / 255.f, cl_col->b / 255.f, cl_col->a / 255.f);
        tea_clear();
    }

    tea_uniform_matrix4fv(shader->world_loc, 1, CA_FALSE, tea_get_matrix(TEA_PROJECTION));
    tea_uniform_matrix4fv(shader->modelview_loc, 1, CA_FALSE, tea_get_matrix(TEA_MODELVIEW));

    ca_i32 i, count;
    count = state->node_end - state->node_start;
    // fprintf(stderr, "State %d (%p)\n\tstart: %d\n\tend: %d\n\tshader: %p\n\ttarget: %p\n", (state - render()->state.pool), state, state->node_start, state->node_end, state->shader, state->target);
    for (i = state->node_start; i < state->node_end+1; i++) {
        ca_RenderNode *node = &render()->node.pool[i];
        ca_Texture *tex = node->texture;
        // te_enum mode = node->draw_mode == CAFE_FILL ? TEA_TRIANGLES : TEA_LINES;
        te_enum mode = gl_modes[node->draw_mode];
        // fprintf(stderr, "\tNode %i (%p)\n\t\tmode %d\n\t\ttexture: %p\n\t\tindex: %d\n\t\tcount: %d\n", i, node, node->draw_mode, tex, node->index, node->count);
        tea_bind_texture(TEA_TEXTURE_2D, tex->handle);

        tea_bind_vao(s_default_vao);
        tea_draw_arrays(mode, node->index, node->count-node->index);
        tea_bind_vao(0);
    }
    tea_use_program(0); 
    tea_bind_fbo(TEA_FRAMEBUFFER, 0);
}

void s_cafe_render_draw(void) {
#if 0
    ca_Texture *target = render()->current_draw_call->target;
    ca_Texture *texture = render()->current_draw_call->texture;
    ca_Shader *shader = render()->current_draw_call->shader;

    te_fbo_t fbo = target ? target->handle : 0;
    te_texture_t tex = texture ? texture->handle : 0;
    te_program_t program = shader ? shader->handle : 0;
    tea_bind_fbo(TEA_FRAMEBUFFER, fbo);
    if (fbo != 0)
        tea_viewport(0, 0, target->width, target->height);
    tea_bind_texture(TEA_TEXTURE_2D, tex);
    tea_use_program(program);
    tea_uniform_matrix4fv(shader->world_loc, 1, CA_FALSE, tea_get_matrix(TEA_PROJECTION));
    tea_uniform_matrix4fv(shader->modelview_loc, 1, CA_FALSE, tea_get_matrix(TEA_MODELVIEW));
    cafe_render_end();
#endif
    if (tea_vertex_is_empty(render()->vertex)) return;
    counter++;
    ca_Shader *shader = state()->shader;
    ca_Texture *target = state()->target;

    ca_i32 width, height;
    width = target->width;
    height = target->height;


    tea_use_program(shader->handle);
    tea_viewport(0, 0, width, height);
    tea_matrix_mode(TEA_PROJECTION);
    tea_load_identity();
    tea_ortho(0, width, height, 0, -1, 1);

    tea_bind_fbo(TEA_FRAMEBUFFER, target->fbo);

    ca_Color *cl_col = &state()->clear_color;

    if (state()->clear_needed) {
        tea_clear_color(cl_col->r / 255.f, cl_col->g / 255.f, cl_col->b / 255.f, cl_col->a / 255.f);
        tea_clear();
    }

    tea_uniform_matrix4fv(shader->world_loc, 1, CA_FALSE, tea_get_matrix(TEA_PROJECTION));
    tea_uniform_matrix4fv(shader->modelview_loc, 1, CA_FALSE, tea_get_matrix(TEA_MODELVIEW));

    ca_i32 i, count;
    count = state()->node_start + state()->node_end;
    fprintf(stderr, "State %d (%p)\n\tindex: %d\n\tcount: %d\n", render()->state.index, state(), state()->node_start, state()->node_end);
    for (i = state()->node_start; i < count; i++) {
        ca_RenderNode *node = &render()->node.pool[i];
        ca_Texture *tex = node->texture;
        // te_enum mode = node->draw_mode == CAFE_FILL ? TEA_TRIANGLES : TEA_LINES;
        te_enum mode = gl_modes[node->draw_mode];
        fprintf(stderr, "\tNode %i (%p)\n\t\tmode %d\n\t\ttexture: %p\n\t\tindex: %d\n\t\tcount: %d\n", i, node, node->draw_mode, tex, node->index, node->count);
        tea_bind_texture(TEA_TEXTURE_2D, tex->handle);

        tea_bind_vao(s_default_vao);
        tea_draw_arrays(mode, node->index, node->count);
        tea_bind_vao(0);
    }
    tea_use_program(0);
    tea_bind_fbo(TEA_FRAMEBUFFER, 0);
    //cafe_render_drawCall(render()->current_draw_call);
}

static void s_cafe_render_draw_reset(void) {
    s_cafe_render_draw();
    cafe_render_begin();
}

void cafe_render_begin(void) {

    ca_i32 width, height;
    width = cafe()->window.width;
    height = cafe()->window.height;
    tea_viewport(0, 0, width, height);
    tea_matrix_mode(TEA_PROJECTION);
    tea_load_identity();
    tea_ortho(0, width, height, 0, -1, 1);
    tea_matrix_mode(TEA_MODELVIEW);
    tea_load_identity();

    render()->state.index = 0;
    memset(state(), 0, sizeof(ca_RenderState));
    state()->shader = s_default_shader;
    state()->target = &s_default_target;

    memset(node(), 0, sizeof(ca_RenderNode));
    node()->draw_mode = CAFE_LINE;
    node()->vertex = render()->vertex;
    node()->texture = &s_white_texture;

    s_default_target.width = width;
    s_default_target.height = height;
    tea_begin_vertex(render()->vertex);
}

void cafe_render_end(void) {
    tea_end_vertex(render()->vertex);
    // s_cafe_render_draw();
    // fprintf(stderr, "State count: %d\n", render()->state.index);
    for (int i = 0; i < render()->state.index+1; i++) {
        s_cafe_render_state(&render()->state.pool[i]);
    }
    /*te_enum mode = gl_modes[render()->current_draw_call->mode];
    tea_bind_vao(s_default_vao);
    tea_draw_vertex(render()->vertex, mode);
    tea_bind_vao(0);*/

}

ca_i32 cafe_render_mode(ca_i32 mode) {
    ca_i32 old_mode = render()->current_draw_call->mode;
    s_cafe_render_draw_reset();
    render()->current_draw_call->mode = mode;
    return old_mode;
}

/*
    Shader -> FBO -> Texture

    Vertex:
    VAO
     - VBO
     - *IBO
*/

void cafe_render_setMode(ca_i32 mode) {
    if (node()->draw_mode == mode) return;
    if (node()->count == 0) {
        node()->draw_mode = mode;
        return;
    }
    ca_RenderNode *old = node();
    old->count = tea_vertex_offset(node()->vertex) - old->index;
    state()->node_end++;
    memcpy(node(), old, sizeof *old);
    node()->draw_mode = mode;
    node()->index = tea_vertex_offset(node()->vertex);
    // if (render()->current_draw_call->mode == mode) return;
    // s_cafe_render_draw_reset();
    // render()->current_draw_call->mode = mode;
}

void cafe_render_setShader(ca_Shader* shader) {
    shader = shader ? shader : s_default_shader;
    // s_cafe_render_draw_reset();
    // render()->current_draw_call->shader = shader;
    if (shader == state()->shader) return;
    ca_i32 count = state()->node_end - state()->node_start;
    if (count == 0 && node()->count == 0) {
        state()->shader = shader;
        return;
    }
    ca_RenderState *old = state();
    ca_RenderNode *old_node = node();
    render()->state.index++;
    memcpy(state(), old, sizeof *old);
    state()->shader = shader;
    state()->clear_needed = CA_FALSE;
    state()->node_start = old->node_end+1;
    state()->node_end = old->node_end+1;

    memcpy(node(), old_node, sizeof *old_node);
    node()->index = tea_vertex_offset(node()->vertex);
    node()->count = 0;
}

void cafe_render_setTarget(ca_Texture *target) {
    target = target ? target : &s_default_target;
    // if (target == render()->current_draw_call->target) return;
    // s_cafe_render_draw_reset();
    // render()->current_draw_call->target = target;
    if (target == state()->target) return;
    ca_i32 count = state()->node_end - state()->node_start;
    if (count == 0 && node()->count == 0) {
        state()->target = target;
        return;
    }
    // fprintf(stderr, "%p == %p ? %d\n", target, state()->target, target == state()->target);
    ca_RenderState *old = state();
    ca_RenderNode *old_node = node();
    render()->state.index++;
    memcpy(state(), old, sizeof *old);
    state()->target = target;
    state()->clear_needed = CA_FALSE;
    state()->node_start = old->node_end+1;
    state()->node_end = old->node_end+1;

    memcpy(node(), old_node, sizeof *old_node);
    node()->index = tea_vertex_offset(node()->vertex);
    node()->count = 0;
}

void cafe_render_setColor(ca_Color color) {
    float col[4];
    ca_u8 *ptr = &color.r;
    for (int i = 0; i < 4; i++)
        col[i] = (float)ptr[i] / 255.f;
    tea_color4f(col[0], col[1], col[2], col[3]);
}

void cafe_render_clear(ca_Color color) {
    // ca_DrawCall *call = render()->current_draw_call;
    memcpy(&state()->clear_color, &color, sizeof color);
    state()->clear_needed = CA_TRUE;

    // call->clear_color.r = color.r;
    // call->clear_color.g = color.g;
    // call->clear_color.b = color.b;
    // call->clear_color.a = color.a;
}

void cafe_render_point(ca_f32 x, ca_f32 y) {
}

void cafe_render_line(ca_f32 x1, ca_f32 y1, ca_f32 x2, ca_f32 y2) {
    cafe_render_setMode(CAFE_LINE);

    tea_vertex2f(x1, y1);
    tea_vertex2f(x2, y2);
    node()->count = tea_vertex_offset(node()->vertex);
}

typedef void(*RenderRectFunction)(ca_f32, ca_f32, ca_f32, ca_f32);

static void s_cafe_render_rect_line(ca_f32 x, ca_f32 y, ca_f32 w, ca_f32 h) {
    tea_vertex2f(x, y);
    tea_vertex2f(x+w, y);

    tea_vertex2f(x+w, y);
    tea_vertex2f(x+w, y+h);

    tea_vertex2f(x+w, y+h);
    tea_vertex2f(x, y+h);

    tea_vertex2f(x, y+h);
    tea_vertex2f(x, y);
}

static void s_cafe_render_rect_fill(ca_f32 x, ca_f32 y, ca_f32 w, ca_f32 h) {
    tea_vertex2f(x, y);
    tea_vertex2f(x+w, y);
    tea_vertex2f(x+w, y+h);

    tea_vertex2f(x, y);
    tea_vertex2f(x, y+h);
    tea_vertex2f(x+w, y+h);
}

void cafe_render_rect(ca_f32 x, ca_f32 y, ca_f32 w, ca_f32 h) {
    RenderRectFunction functions[] = {
        s_cafe_render_rect_line,
        s_cafe_render_rect_fill
    };
    ca_i32 mode = node()->draw_mode;
    mode = mode > 1 ? 1 : mode;
    functions[mode](x, y, w, h);
    node()->count = tea_vertex_offset(node()->vertex);
}

typedef void(*RenderTriangleFunction)(ca_f32 x1, ca_f32 y1, ca_f32 x2, ca_f32 y2, ca_f32 x3, ca_f32 y3);

static void s_cafe_render_triangle_line(ca_f32 x1, ca_f32 y1, ca_f32 x2, ca_f32 y2, ca_f32 x3, ca_f32 y3) {
    tea_vertex2f(x1, y1);
    tea_vertex2f(x2, y2);

    tea_vertex2f(x2, y2);
    tea_vertex2f(x3, y3);

    tea_vertex2f(x3, y3);
    tea_vertex2f(x1, y1);
}

static void s_cafe_render_triangle_fill(ca_f32 x1, ca_f32 y1, ca_f32 x2, ca_f32 y2, ca_f32 x3, ca_f32 y3) {
    tea_vertex2f(x1, y1);
    tea_vertex2f(x2, y2);
    tea_vertex2f(x3, y3);  
}

void cafe_render_triangle(ca_f32 x1, ca_f32 y1, ca_f32 x2, ca_f32 y2, ca_f32 x3, ca_f32 y3) {
    RenderTriangleFunction functions[] = {
        s_cafe_render_triangle_line,
        s_cafe_render_triangle_fill
    };
    ca_i32 mode = node()->draw_mode;
    mode = mode > 1 ? 1 : mode;
    // fprintf(stderr, "Testando: %d mode\n", mode);
    functions[mode](x1, y1, x2, y2, x3, y3);
    node()->count = tea_vertex_offset(node()->vertex);
}

typedef void(*RenderCircleFunction)(ca_f32, ca_f32, ca_f32);

static void s_cafe_render_circle_line(ca_f32 x, ca_f32 y, ca_f32 radius) {
    int sides = 32;
    double pi2 = 2.0 * TEA_PI;
    int i;
    for (i = 0; i < sides; i++) {
        float tetha = (i * pi2) / sides;
        tea_vertex2f(x + (cosf(tetha) * radius), y + (sinf(tetha) * radius));
        tetha = ((i+1) * pi2) / sides;
        tea_vertex2f(x + (cosf(tetha) * radius), y + (sinf(tetha) * radius));
    }
}

static void s_cafe_render_circle_fill(ca_f32 x, ca_f32 y, ca_f32 radius) {
    int sides = 32;
    double pi2 = 2.0 * TEA_PI;
    int i;
    for (i = 0; i < sides; i++) {
        float tetha = (i * pi2) / sides;
        float tetha2 = ((i+1) * pi2) / sides;
        /* tea_vertex2f(x, y);
        tea_vertex2f(x + (cosf(tetha) * radius), y + (sinf(tetha) * radius));
        tea_vertex2f(x + (cosf(tetha) * radius), y + (sinf(tetha) * radius)); */
        cafe_render_triangle(x, y, 
                             x + (cosf(tetha) * radius), y + (sinf(tetha) * radius),
                             x + (cosf(tetha2) * radius), y + (sinf(tetha2) * radius));
    }
}

void cafe_render_circle(ca_f32 x, ca_f32 y, ca_f32 radius) {
    RenderCircleFunction functions[] = {
        s_cafe_render_circle_line,
        s_cafe_render_circle_fill
    };
    ca_i32 mode = node()->draw_mode;
    mode = mode > 1 ? 1 : mode;
    functions[mode](x, y, radius);
    node()->count = tea_vertex_offset(node()->vertex);
}

void cafe_render_texture(ca_Texture *tex, ca_Rect *dest, ca_Rect *src) {
    if (!tex)
        return;
    // if (tex != render()->current_draw_call->texture)
    //     s_cafe_render_draw_reset();
    if (node()->count == 0) {
        node()->texture = tex;
    } else if (tex != node()->texture) {
        ca_RenderNode *old = node();
        old->count = tea_vertex_offset(render()->vertex) - old->index;
        state()->node_end++;
        memcpy(node(), old, sizeof *old);
        node()->texture = tex;
        node()->index = tea_vertex_offset(render()->vertex);
    }
    // render()->current_draw_call->texture = tex;
    ca_Rect d, s;
    d.x = 0;
    d.y = 0;
    d.w = tex->width;
    d.h = tex->height;
    s.x = 0;
    s.y = 0;
    s.w = tex->width;
    s.h = tex->height;
    if (dest) memcpy(&d, dest, sizeof *dest);
    if (src) memcpy(&s, src, sizeof *src);

    ca_Vec2 coord[4] = {
        { 0.f, 0.f },
        { 1.f, 0.f },
        { 1.f, 1.f },
        { 0.f, 1.f }
    };

    coord[0][0] = coord[3][0] = s.x / tex->width;
    coord[0][1] = coord[1][1] = s.y / tex->height;

    coord[1][0] = coord[2][0] = (s.x + s.w) / tex->width;
    coord[2][1] = coord[3][1] = (s.y + s.h) / tex->height;

    if (tex->usage == CAFE_TEXTURE_TARGET) {
        coord[0][1] = coord[1][1] = (s.y + s.h) / tex->height;
        coord[2][1] = coord[3][1] = s.y / tex->height;
    }

    tea_texcoord2f(coord[0][0], coord[0][1]);
    tea_vertex2f(d.x, d.y);
    tea_texcoord2f(coord[1][0], coord[1][1]);
    tea_vertex2f(d.x + d.w, d.y);
    tea_texcoord2f(coord[2][0], coord[2][1]);
    tea_vertex2f(d.x + d.w, d.y + d.h);

    tea_texcoord2f(coord[0][0], coord[0][1]);
    tea_vertex2f(d.x, d.y);
    tea_texcoord2f(coord[3][0], coord[3][1]);
    tea_vertex2f(d.x, d.y+d.h);
    tea_texcoord2f(coord[2][0], coord[2][1]);
    tea_vertex2f(d.x+d.w, d.y+d.h);
    node()->count = tea_vertex_offset(node()->vertex);
}

void cafe_render_drawCall(ca_DrawCall *draw_call) {
    if (!draw_call)
        return;

    ca_Texture *tex = draw_call->texture;
    ca_Texture *target = draw_call->target;
    ca_Shader *shader = draw_call->shader;

    te_texture_t ttex = tex ? tex->handle : 0;
    te_fbo_t fbo = target ? target->fbo : 0;
    te_program_t program = shader ? shader->handle : 0;

    tea_bind_fbo(TEA_FRAMEBUFFER, fbo);
    ca_Color *cl_col = &draw_call->clear_color;
    if (fbo != 0) {
        tea_clear_color(cl_col->r / 255.f, cl_col->g / 255.f, cl_col->b / 255.f, cl_col->a / 255.f);
        tea_clear();
    }
    ca_i32 width, height;
    width = target->width;
    height = target->height;

    int mode = render()->current_draw_call->mode;
    
    tea_viewport(0, 0, width, height);
    tea_matrix_mode(TEA_PROJECTION);
    tea_load_identity();
    tea_ortho(0, width, height, 0, -1, 1);

    tea_bind_texture(TEA_TEXTURE_2D, ttex);
    tea_use_program(program);
    tea_uniform_matrix4fv(shader->world_loc, 1, CA_FALSE, tea_get_matrix(TEA_PROJECTION));
    tea_uniform_matrix4fv(shader->modelview_loc, 1, CA_FALSE, tea_get_matrix(TEA_MODELVIEW));
    cafe_render_end();
    tea_use_program(0);
    tea_bind_texture(TEA_TEXTURE_2D, 0);
    tea_bind_fbo(TEA_FRAMEBUFFER, 0);
}

/*****************
 * Texture       *
 ****************/

ca_Texture* cafe_render_createTexture(ca_i32 width, ca_i32 height, ca_u8* data, ca_i32 usage) {
    ca_Texture *texture = NULL;
    
    texture = (ca_Texture*)malloc(sizeof *texture);
    texture->handle = tea_texture2D(data, width, height, TEA_RGBA);
    texture->width = width;
    texture->height = height;
    texture->filter[0] = CAFE_NEAREST;
    texture->filter[1] = CAFE_NEAREST;
    texture->wrap[0] = CAFE_CLAMP;
    texture->wrap[1] = CAFE_CLAMP;
    texture->usage = usage;

    if (usage == CAFE_TEXTURE_TARGET) {
        texture->fbo = tea_fbo();
        tea_bind_fbo(TEA_FRAMEBUFFER, texture->fbo);
        tea_fbo_texture(TEA_FRAMEBUFFER, TEA_COLOR_ATTACHMENT0, texture->handle);
        tea_bind_fbo(TEA_FRAMEBUFFER, 0);
    }

    cafe_texture_setFilter(texture, texture->filter[0], texture->filter[1]);
    cafe_texture_setWrap(texture, texture->wrap[0], texture->wrap[1]);
    return texture;
}

ca_Texture* cafe_render_loadTexture(const char *path) {
    ca_Texture *texture = NULL;
    if (!path) return texture;

    la_file_t *file = la_fopen(path, LA_READ_MODE);
    la_header_t h;
    la_fheader(file, &h);

    char data[h.size + 1];
    la_fread(file, data, h.size);
    data[h.size] = '\0';

    ca_i32 width, height;
    ca_i32 channels;
    unsigned char* pixels = stbi_load_from_memory(data, h.size, &width, &height, &channels, STBI_rgb_alpha);

    texture = cafe_render_createTexture(width, height, pixels, 0);
    stbi_image_free(pixels);

    return texture;
}

void cafe_texture_destroy(ca_Texture *texture) {
    if (!texture)
        return;
    tea_free_texture(texture->handle);
    free(texture);
}

void cafe_texture_setFilter(ca_Texture *tex, ca_i32 filter_min, ca_i32 filter_mag) {
    if (!tex) return;

    te_enum fmin = filter_min == CAFE_NEAREST ? TEA_NEAREST : TEA_LINEAR_MIPMAP_LINEAR;
    te_enum fmag = filter_mag == CAFE_NEAREST ? TEA_NEAREST : TEA_LINEAR_MIPMAP_LINEAR;

    tea_bind_texture(TEA_TEXTURE_2D, tex->handle);
    tea_tex_parameteri(TEA_TEXTURE_MIN_FILTER, fmin);
    tea_tex_parameteri(TEA_TEXTURE_MAG_FILTER, fmag);
    tea_bind_texture(TEA_TEXTURE_2D, 0);
}

void cafe_texture_setWrap(ca_Texture *tex, ca_i32 wrap_s, ca_i32 wrap_t) {
    if (!tex) return;
    te_enum ws = wrap_s == CAFE_CLAMP ? TEA_CLAMP_TO_EDGE : TEA_REPEAT;
    te_enum wt = wrap_t == CAFE_CLAMP ? TEA_CLAMP_TO_EDGE : TEA_REPEAT;

    tea_bind_texture(TEA_TEXTURE_2D, tex->handle);
    tea_tex_parameteri(TEA_TEXTURE_WRAP_S, ws);
    tea_tex_parameteri(TEA_TEXTURE_WRAP_T, wt);
    tea_bind_texture(TEA_TEXTURE_2D, 0);
}

ca_i32 cafe_texture_getWidth(ca_Texture *tex) {
    if (!tex) return -1;
    return tex->width;
}

ca_i32 cafe_texture_getHeight(ca_Texture *tex) {
    if (!tex) return -1;
    return tex->height;
}

void cafe_texture_getSize(ca_Texture *tex, ca_i32 *width, ca_i32 *height) {
    if (!tex) return;
    if (width) *width = tex->width;
    if (height) *height = tex->height;
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

    te_shader_t shaders[2];
    shaders[0] = tea_shader(TEA_VERTEX_SHADER, vert_source);
    shaders[1] = tea_shader(TEA_FRAGMENT_SHADER, frag_source);

    shader->handle = tea_program(2, shaders);
    tea_delete_shader(shaders[0]);
    tea_delete_shader(shaders[1]);
    shader->world_loc = tea_get_uniform_location(shader->handle, "u_World");
    shader->modelview_loc = tea_get_uniform_location(shader->handle, "u_ModelView");
    shader->texture_loc = tea_get_uniform_location(shader->handle, "u_Texture");
    return shader;
}

void cafe_shader_destroy(ca_Shader *shader) {
    if (!shader) return;
    tea_free_program(&shader->handle);
    free(shader);
}

void cafe_shader_setUniform1f(ca_Shader *shader, const char *name, ca_f32 value) {
    if (!shader)
	return;
    int loc = tea_get_uniform_location(shader->handle, name);

    te_shader_t current = state()->shader ? state()->shader->handle : 0;
    tea_use_program(shader->handle);
    tea_uniform1f(loc, value);
    tea_use_program(current);
}

void cafe_shader_setUniform2f(ca_Shader *shader, const char *name, ca_f32 v0, ca_f32 v1) {
    if (!shader)
	return;
    int loc = tea_get_uniform_location(shader->handle, name);
    te_shader_t current = state()->shader ? state()->shader->handle : 0;
    tea_use_program(shader->handle);
    tea_uniform2f(loc, v0, v1);
    tea_use_program(current);

}

void cafe_shader_setUniform3f(ca_Shader *shader, const char *name, ca_f32 v0, ca_f32 v1, ca_f32 v2) {
    if (!shader)
	return;
    int loc = tea_get_uniform_location(shader->handle, name);
    te_shader_t current = state()->shader ? state()->shader->handle : 0;
    tea_use_program(shader->handle);
    tea_uniform3f(loc, v0, v1, v2);
    tea_use_program(current);
}

void cafe_shader_setUniform4f(ca_Shader *shader, const char *name, ca_f32 v0, ca_f32 v1, ca_f32 v2, ca_f32 v3) {
    if (!shader)
	return;
    int loc = tea_get_uniform_location(shader->handle, name);
    te_shader_t current = state()->shader ? state()->shader->handle : 0;
    tea_use_program(shader->handle);
    tea_uniform4f(loc, v0, v1, v2, v3);
    tea_use_program(current);
}

