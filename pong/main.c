#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <SDL2/SDL.h>

const int WIN_WIDTH  = 640;
const int WIN_HEIGHT = 480;

const double FRAME_TIME_MS = 1000.0/60;

const int PADDLE_WIDTH  = 10;
const int PADDLE_HEIGHT = 100;
const int BALL_SIZE     = 10;

const int PADDLE_XMARGIN = 10;

const int PADDLE_SPEED = 5;
const int BALL_SPEED   = 4;

const int LINE_WIDTH =  10;
const int LINE_HEIGHT = 50;

// in percentage
const int LEFT_SCORE_X  = 25;
const int RIGHT_SCORE_X = 75;

const int PIXEL_SIZE = 5;

// each digit is 4x5
const int DIGITS[10][5][4] = {
    {{1,1,1,1},{1,0,0,1},{1,0,0,1},{1,0,0,1},{1,1,1,1}},
    {{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1}},
    {{0,1,1,0},{1,0,0,1},{0,0,1,0},{0,1,0,0},{1,1,1,1}},
    {{0,1,1,0},{0,0,0,1},{0,1,1,0},{0,0,0,1},{0,1,1,0}},
    {{1,0,1,0},{1,0,1,0},{1,1,1,1},{0,0,1,0},{0,0,1,0}},
    {{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}},
    {{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}},
    {{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}},
    {{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}},
    {{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}}
};

typedef struct {
    int32_t y;
    uint32_t score;
} paddle_t;

typedef struct {
    int32_t x, y;
    int32_t vx, vy;
} ball_t;

typedef struct {
    paddle_t left;
    paddle_t right;

    ball_t ball;
} gamestate_t;

gamestate_t state;

void die(const char* msg){
    fprintf(stderr, "%s: %s\n", msg, SDL_GetError());
    exit(1);
}

void reset_game(){
    int paddley = (WIN_HEIGHT - PADDLE_HEIGHT) / 2;
    state.left.y = state.right.y = paddley;

    state.ball.x = (WIN_WIDTH - BALL_SIZE) / 2;
    state.ball.y = (WIN_HEIGHT - BALL_SIZE) / 2;

    uint32_t r = rand()%100;
    if(r<50){
        state.ball.vx = BALL_SPEED;
    } else {
        state.ball.vx = -BALL_SPEED;
    }

    r = rand()%100;
    if(r<50){
        state.ball.vy = BALL_SPEED;
    } else {
        state.ball.vy = -BALL_SPEED;
    }

}

void init(){
    srand(time(NULL));

    reset_game();

    state.left.score = 0;
    state.right.score = 0;
}

void update(uint64_t ticks, SDL_Event evt){
    // keypresses
    if(evt.type == SDL_KEYDOWN) {
        switch(evt.key.keysym.sym){
            case SDLK_UP:
                if(state.left.y > 0){
                    state.left.y -= PADDLE_SPEED;
                }
                break;
            case SDLK_DOWN:
                if(state.left.y + PADDLE_HEIGHT < WIN_HEIGHT){
                    state.left.y += PADDLE_SPEED;
                }
                break;
        }
    }

    // collisions
    if(state.ball.x <= PADDLE_XMARGIN + PADDLE_WIDTH
        && state.ball.y >= state.left.y
        && state.ball.y <= state.left.y + PADDLE_HEIGHT){
        state.ball.vx *= -1;
    }

    if(state.ball.x >= WIN_WIDTH - (PADDLE_XMARGIN + PADDLE_WIDTH)
        && state.ball.y >= state.right.y
        && state.ball.y <= state.right.y + PADDLE_HEIGHT){
        state.ball.vx *= -1;
    }

    // scoring
    if(state.ball.x <= 0){
        state.right.score++;
        reset_game();
        SDL_Delay(1000);
        return;
    }

    if(state.ball.x >= WIN_WIDTH){
        state.left.score++;
        reset_game();
        SDL_Delay(1000);
        return;
    }

    // ball movemwent

    state.ball.x += state.ball.vx;
    state.ball.y += state.ball.vy;

    // ball bouncing
    if(state.ball.y < 0){
        state.ball.vy *= -1;
    }

    if(state.ball.y + BALL_SIZE > WIN_HEIGHT){
        state.ball.vy *= -1;
    }
}

void draw_score(SDL_Surface* srf){
    const int leftx  = LEFT_SCORE_X * WIN_WIDTH / 100;
    const int rightx = RIGHT_SCORE_X * WIN_WIDTH / 100;

    const uint32_t white = SDL_MapRGB(srf->format, 0xFF, 0xFF, 0xFF);
    SDL_Rect rect;

    rect.w = rect.h = PIXEL_SIZE;
    
    rect.y = PADDLE_XMARGIN;

    int i, j;
    for(i=0;i<5;i++){
        rect.x = leftx;
        for(j=0;j<4;j++){
            if(DIGITS[state.left.score][i][j]){
                SDL_FillRect(srf, &rect, white);
            }

            rect.x += PIXEL_SIZE;
        }
        rect.y += PIXEL_SIZE;
    }

    rect.y = PADDLE_XMARGIN;

    for(i=0;i<5;i++){
        rect.x = rightx;
        for(j=0;j<4;j++){
            if(DIGITS[state.right.score][i][j]){
                SDL_FillRect(srf, &rect, white);
            }

            rect.x += PIXEL_SIZE;
        }
        rect.y += PIXEL_SIZE;
    }
}

void draw_dashed_line(SDL_Surface* srf){
    const int linex = (WIN_WIDTH - LINE_WIDTH) / 2;

    const uint32_t white = SDL_MapRGB(srf->format, 0xFF, 0xFF, 0xFF);
    int y;
    SDL_Rect rect;
    rect.x = linex;
    rect.w = LINE_WIDTH;
    rect.h = LINE_HEIGHT;

    for(y = 0; y < WIN_HEIGHT; y += 2*LINE_HEIGHT){
        rect.y = y;
        SDL_FillRect(srf, &rect, white);
    }
}

void draw_game(SDL_Surface* srf){
    const int leftx  = PADDLE_XMARGIN;
    const int rightx = WIN_WIDTH - PADDLE_XMARGIN - PADDLE_WIDTH;

    SDL_Rect left, right, ball;

    left.x = leftx;
    left.y = state.left.y;

    right.x = rightx;
    right.y = state.right.y;

    ball.x = state.ball.x;
    ball.y = state.ball.y;

    left.w = right.w = PADDLE_WIDTH;
    left.h = right.h = PADDLE_HEIGHT;

    ball.w = ball.h = BALL_SIZE;

    const uint32_t white = SDL_MapRGB(srf->format, 0xFF, 0xFF, 0xFF);

    SDL_FillRect(srf, &left, white);
    SDL_FillRect(srf, &right, white);
    SDL_FillRect(srf, &ball, white);
}

void draw(SDL_Window* win, SDL_Surface* srf){
    // draw the black bg
    SDL_FillRect(srf, NULL, SDL_MapRGB(srf->format, 0x00, 0x00, 0x00));

    draw_score(srf);
    draw_dashed_line(srf);
    draw_game(srf);
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

    srf = SDL_GetWindowSurface(win);

    bool running = 1;
    uint64_t ticks = 0;

    init();

    while(running){
        SDL_Event evt;
        uint32_t evt_count = 0;

        int start = SDL_GetTicks();

        while(SDL_PollEvent(&evt)){
            evt_count++;
            if(evt.type == SDL_QUIT){
                running = 0;
                break;
            }

            update(ticks, evt);
        }

        if(!evt_count){
            SDL_Event none;
            none.type = 0;
            update(ticks, none);
        }

        draw(win, srf);

        // paint the window
        SDL_UpdateWindowSurface(win);

        ticks++;

        int diff = SDL_GetTicks() - start;
        int delay = FRAME_TIME_MS - diff;
                        
        if (delay > 0) {
            SDL_Delay(delay);
        }
    }

    SDL_DestroyWindow(win);
    SDL_Quit();

    return 0;
}

