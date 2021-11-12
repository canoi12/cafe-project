#include "cafe.h"
#include <SDL2/SDL.h>

void update(float dt)
{
    // Update the game
    if (cafe_keyboard_isDown(CAFEK_UP))
        fprintf(stderr, "Up\n");
}

int main(int argc, char **argv) {
    ca_Config config = cafe_config("Hello Window", 640, 480);
    cafe_open(&config);
    cafe_run(update);
    cafe_close();

    return 0;
}
