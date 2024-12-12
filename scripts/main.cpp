#define SDL_MAIN_HANDLED
#include "engine.h"

int main() {
    SDL_SetMainReady();
    Engine eng;
    eng.Run();
}
