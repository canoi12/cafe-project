#include "cafe.h"
#include "tea.h"
#include <SDL2/SDL.h>

float x, y;

const char *_frag_shader =
"vec4 pixel(vec4 color, vec2 tex_coord, sampler2D texture) {\n"
"    return color;\n"
"}\n";

ca_Shader *shader;

void update(float dt)
{
    // Update the game
    if (cafe_keyboard_isDown(CAFEK_LEFT))
	x -= 10.f;
    else if (cafe_keyboard_isDown(CAFEK_RIGHT))
	x += 10.f;

    if (cafe_keyboard_isDown(CAFEK_UP))
	y -= 10.f;
    else if (cafe_keyboard_isDown(CAFEK_DOWN))
	y += 10.f;

    tea_color4f(1.f, 0.f, 0.f, 1.f);
    /*tea_vertex2f(-0.5f, -0.5f);
    tea_vertex2f(0.5f, -0.5f);
    tea_vertex2f(0.0f, 0.5f);*/
    tea_vertex2f(x+64, y+64);
    tea_color3f(0.f, 1.f, 0.f);
    tea_vertex2f(x+32, y+96);
    tea_color3f(0.f, 0.f, 1.f);
    tea_vertex2f(x+96, y+96);
}

int main(int argc, char **argv) {
    ca_Config config = cafe_config("Hello Window", 640, 480);
    x = y = 0;
    cafe_open(&config);

    shader = cafe_render_createShader(NULL, _frag_shader);

    cafe_run(update);
    cafe_close();

    return 0;
}
