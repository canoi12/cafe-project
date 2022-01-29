#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include "tea.h"

const char *vert =
"#version 120\n"
"uniform mat4 uProj;\n"
"uniform mat4 uModelView;\n"
"attribute vec3 in_Pos;\n"
"attribute vec4 in_Color;\n"
"attribute vec2 in_TexCoord;\n"
"varying vec4 fragColor;\n"
"varying vec2 texCoord;\n"
"void main() {\n"
"    fragColor = in_Color;\n"
"    texCoord = in_TexCoord;\n"
"    gl_Position = uProj * uModelView * vec4(in_Pos, 1.0);\n"
"}\n";

const char *frag =
"#version 120\n"
"varying vec4 fragColor;\n"
"varying vec2 texCoord;\n"
"uniform sampler2D uTexture;\n"
"void main() {\n"
"    gl_FragColor = fragColor * texture2D(uTexture, texCoord);\n"
"}\n";

int main(int argc, char **argv) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        return 1;
    }
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    ////SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
    SDL_Window *window = SDL_CreateWindow("Tea", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
    if (window == NULL) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        return 1;
    }
    SDL_GLContext context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, context);
    SDL_Event event;

    te_config_t config = tea_config();
    tea_init(&config);

    //te_shader_t shader = tea_shader(vert, frag);
    te_shader_t fragShader = tea_shader(TEA_FRAGMENT_SHADER, frag);
    te_shader_t vertShader = tea_shader(TEA_VERTEX_SHADER, vert);
    te_shader_t shaders[] = {vertShader, fragShader};
    te_program_t program = tea_program(2, shaders);

    te_texture_t white = tea_white_texture();
    int projLoc, modViewLoc;
    projLoc = tea_get_uniform_location(program, "uProj");
    modViewLoc = tea_get_uniform_location(program, "uModelView");

    //te_vertex_t *vertex = tea_vertex(NULL);

    while (event.type != SDL_QUIT) {
        SDL_PollEvent(&event);
        tea_clear_color(0.3, 0.4, 0.4, 1.0);
        tea_clear();

        tea_viewport(0, 0, 640, 480);
        tea_matrix_mode(TEA_PROJECTION);
        tea_load_identity();
        tea_perspective(45.0, 640.0 / 480.0, 0.1, 100.0);
        tea_matrix_mode(TEA_MODELVIEW);
        tea_load_identity();
        tea_translatef(0, 0, -2);
        //tea_translatef(-0.5, 0, 0);

        tea_use_program(program);
        tea_bind_texture(TEA_TEXTURE_2D, white);
        tea_uniform_matrix4fv(projLoc, 1, 0, tea_get_matrix(TEA_PROJECTION));
        tea_uniform_matrix4fv(modViewLoc, 1, 0, tea_get_matrix(TEA_MODELVIEW));
        tea_begin(TEA_LINES);
        //tea_begin_vertex(vertex);
#if 0
        tea_color3f(1.f, 0.f, 0.f);
        tea_vertex2f(-0.5f, -0.5f);
        tea_color3f(0.f, 1.f, 0.f);
        tea_vertex2f(0.5f, -0.5f);
        tea_color3f(0.f, 0.f, 1.f);
        tea_vertex2f(0.0f, 0.5f);
#else
        tea_color3f(1.f, 0.f, 0.f);
        tea_vertex2f(-0.5f, -0.5f);
        tea_color3f(0.f, 1.f, 0.f);
        tea_vertex2f(0.5f, -0.5f);
        tea_vertex2f(0.5f, -0.5f);

        tea_color3f(0.f, 0.f, 1.f);
        tea_vertex2f(0.0f, 0.5f);
        tea_vertex2f(0.0f, 0.5f);

        tea_color3f(1.f, 0.f, 0.f);
        tea_vertex2f(-0.5f, -0.5f);
#endif
        tea_end();
        //tea_end_vertex(vertex);
        //tea_draw_vertex(vertex, TEA_TRIANGLES);
        tea_use_program(0);

        SDL_GL_SwapWindow(window);
    }

    return 0;
}
