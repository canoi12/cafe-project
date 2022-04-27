#include "cafe.h"
#include "tea.h"
#include <SDL2/SDL.h>

float x, y;

const char *_frag_shader =
"vec4 pixel(vec4 color, vec2 tex_coord, sampler2D sample) {\n"
"    return color * texture(sample, vec2(tex_coord.y, tex_coord.x));\n"
"}\n";

ca_Shader *shader;
ca_Texture *texture;
ca_Texture *target;

void update(float dt)
{
    // cafe_render_setShader(shader);
    // Update the game
    if (cafe_keyboard_isDown(CAFEK_LEFT))
	x -= 10.f;
    else if (cafe_keyboard_isDown(CAFEK_RIGHT))
	x += 10.f;

    if (cafe_keyboard_isDown(CAFEK_UP))
	y -= 10.f;
    else if (cafe_keyboard_isDown(CAFEK_DOWN))
	y += 10.f;
    cafe_render_setColor(CAFE_RGB(255, 255, 255));
    /*
    tea_color4f(1.f, 0.f, 0.f, 1.f);
    tea_vertex2f(x+64, y+64);
    tea_color3f(0.f, 1.f, 0.f);
    tea_vertex2f(x+32, y+96);
    tea_color3f(0.f, 0.f, 1.f);
    tea_vertex2f(x+96, y+96);
    */
    cafe_render_setTarget(target);
    cafe_render_setMode(CAFE_LINE);
    cafe_render_clear(CAFE_RGB(0, 0, 0));
    cafe_render_triangle(x+32, y, x, y+32, x+64, y+32);
    cafe_render_rect(x+64, y, 8, 8);
    cafe_render_setTarget(NULL);

    cafe_render_setMode(CAFE_FILL);
    cafe_render_circle(0, 0, 32);

    //cafe_render_setShader(shader);
    ca_Rect dest;
    dest.x = 0;
    dest.y = 0;
    dest.w = 320;
    dest.h = 190;
    cafe_render_texture(target, &dest, NULL);
}

int main(int argc, char **argv) {
    ca_Config config = cafe_config(NULL, 640, 480);
    x = y = 0;
    if (!cafe_open(&config)) {
        fprintf(stderr, "Failed to init cafe\n");
        return -1;
    }

    shader = cafe_render_createShader(NULL, _frag_shader);
    texture = cafe_render_loadTexture("penguin.jpg");

    target = cafe_render_createTexture(160, 95, NULL, CAFE_TEXTURE_TARGET);

    cafe_run(NULL);
    cafe_close();

    return 0;
}
