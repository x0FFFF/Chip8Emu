#include <SDL.h>
#include <stdio.h>
#include <math.h>

#include "chip8.h"

#define DEBUG_MODE

#define PIXEL_SIZE 15

#define SCR_WIDTH  PIXEL_SIZE * 64
#define SCR_HEIGHT PIXEL_SIZE * 32

int main(int argc, char* argv[]) {
    char* path = NULL;

#ifdef DEBUG_MODE
    // for convenience store path to a test rom
    path = "../ROMS/PONG";

#endif

    int i = 0, j = 0;
    // use gcd to figure out the best size for pixels
    int pixel_size = PIXEL_SIZE;
    // will store pixel position when initializing pixels
    int x_offset = 0, y_offset = 0;

    // Event loop
    SDL_Event e;
    int running = 1;

    if (CHIP8_loadROM(path) == FAILED_TO_LOAD_ROM) {
        printf("There was an error while loading ROM at %s\n", path);
        return 1;
    }

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    // Create a window
    SDL_Window* window = SDL_CreateWindow(
        "Black Window",                  // window title
        SDL_WINDOWPOS_CENTERED,         // initial x position
        SDL_WINDOWPOS_CENTERED,         // initial y position
        SCR_WIDTH, SCR_HEIGHT,                        // width and height
        SDL_WINDOW_SHOWN                // flags
    );

    if (!window) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Set draw color to white (R, G, B, A)
    //SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 255);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    // Clear screen with the draw color
    SDL_RenderClear(renderer);

    // Update the screen
    SDL_RenderPresent(renderer);
    // declare our screen pixels with rects
    // since we have 64x32 screen, we need 2048 rects
    SDL_Rect pixels[SCR_WIDTH][SCR_HEIGHT];

    // init all the pixels with rects
    for (i = 0; i < SCR_WIDTH; i++) {
        for (j = 0; j < SCR_HEIGHT; j++) {
            // init size
            pixels[i][j].w = pixel_size;
            pixels[i][j].h = pixel_size;
            // init coords
            // we'll calculate the coordinates as offset from last pixel on the line
            pixels[i][j].x = x_offset;
            pixels[i][j].y = y_offset;

            x_offset += pixel_size;
        }

        // reset for each new line
        x_offset = 0;
        y_offset += pixel_size;
    }

    // game loop
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = 0;
            }
        }
        SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF );

        // testing to render a single pixel
        SDL_RenderFillRect(renderer, &(pixels[0][0]));
        SDL_SetRenderDrawColor( renderer, 0, 0, 0, 0 );
        // Clear the screen with black color and update
        //SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);

        SDL_Delay(16); // ~60 FPS
    }

    // Clean up
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}