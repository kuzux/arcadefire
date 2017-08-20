#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

const int WIN_WIDTH  = 640;
const int WIN_HEIGHT = 480;

void die(const char* msg){
    fprintf(stderr, "%s: %s\n", msg, SDL_GetError());
    exit(1);
}

int main(int argc, char** argv){
    SDL_Window* win = NULL;
    SDL_Surface* srf = NULL;

    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        die("sdl_init");
    }

    win = SDL_CreateWindow(
        "Hello SDL",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        WIN_WIDTH,
        WIN_HEIGHT,
        SDL_WINDOW_SHOWN);

    if(!win){
        die("sdl_createwindow");
    }

    // sleep for 2s
    SDL_Delay(2000);

    SDL_DestroyWindow(win);
    SDL_Quit();

    return 0;
}
