#include <stdio.h>
#include <SDL.h>
#include "chip8.h"

#define PIXEL_SiZE 10
// these will define SDL_Window dimensions
#define WINDOW_HEIGHT (CHIP8_SCREEN_HEIGHT * PIXEL_SiZE)
#define WINDOW_WIDTH  (CHIP8_SCREEN_WIDTH  * PIXEL_SiZE)

static void initPixels(SDL_Rect pixels[][CHIP8_SCREEN_WIDTH], int pixel_size, int rows, int cols);
static void renderScreen(SDL_Renderer* renderer, SDL_Rect pixels[][CHIP8_SCREEN_WIDTH], BYTE vmem[][CHIP8_SCREEN_WIDTH]);

int main(void)
{
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Event event;

    SDL_Rect pixels[CHIP8_SCREEN_HEIGHT][CHIP8_SCREEN_WIDTH];

    // is used for the main loop
    int is_running = 1;

    // init SDL and check if SDL was initialized correctly
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Failed to initialize SDL library.");
        exit(-1);
    }

    // init the main window
    window = SDL_CreateWindow("Chip8Emu",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        0
        );

    // now check if the main window was created
    if (!window)
    {
        printf("Failed to create window.");
        exit(-1);
    }

    initPixels(pixels, PIXEL_SiZE, CHIP8_SCREEN_HEIGHT, CHIP8_SCREEN_WIDTH);

    // now let's set up the renderer
    renderer = SDL_CreateRenderer(window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
        );
    // check if failed to get surface
    if (!renderer)
    {
        printf("Failed to create window renderer.");
        exit(-1);
    }

    // only for debug
    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, SDL_ALPHA_OPAQUE);

    vmem[0][0] = 1;
    vmem[1][1] = 1;
    vmem[2][2] = 1;
    vmem[3][3] = 1;

    renderScreen(renderer, pixels, vmem);

    SDL_RenderPresent(renderer);
    ////////////////// end of "for debug" area

    // main game loop
    while (is_running)
    {
        if (SDL_PollEvent(&event) > 0)
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    is_running = 0;
                    break;
                default: break;
            }
        }

        SDL_UpdateWindowSurface(window);
    }

    // release the renderer
    SDL_DestroyRenderer(renderer);

    return 0;
}

/// Initializes a 2d array of pixels with SDL_Rect
/// @param pixels Pixel 2d array to be initialized
/// @param pixel_size The size of each pixel on the screen
/// @param rows Number of rows in the array
/// @param cols Number of columns in the array
void initPixels(SDL_Rect pixels[][CHIP8_SCREEN_WIDTH], int pixel_size, int rows, int cols) {
    int i = 0;
    int j = 0;

    int x_offset = 0;
    int y_offset = 0;

    for (i = 0; i < rows; i++)
    {
        // init each row
        for (j = 0; j < cols; j++)
        {
            pixels[i][j].x = x_offset;
            pixels[i][j].y = y_offset;

            pixels[i][j].h = pixel_size;
            pixels[i][j].w = pixel_size;

            // move the offset to the right to fit the next pixel inside the row
            x_offset += pixel_size;
        }

        // after finishing the row, reset x_offset to start from the left
        x_offset = 0;
        // also increase y_offset to fit the next row
        y_offset += pixel_size;
    }
}

/// Renders all the pixels according to their state in video memory to the renderer
/// @param renderer Renderer
/// @param pixels Array of SDL_rect, represents the pixels on the screen
/// @param vmem video memory of CHIP8
void renderScreen(SDL_Renderer* renderer, SDL_Rect pixels[][CHIP8_SCREEN_WIDTH], BYTE vmem[][CHIP8_SCREEN_WIDTH])
{
    int i = 0;
    int j = 0;

    for (i = 0; i < CHIP8_SCREEN_HEIGHT; i++)
    {
        for (j = 0; j < CHIP8_SCREEN_WIDTH; j++)
        {
            // if pixel at i, j is on, the draw it with white color
            if (vmem[i][j])
                SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, SDL_ALPHA_OPAQUE);
            // else draw it with black
            else
                SDL_SetRenderDrawColor(renderer, 0x0, 0x0, 0x0, SDL_ALPHA_OPAQUE);

            // now render the pixel on the renderer with correct color we set above
            SDL_RenderFillRect(renderer, &pixels[i][j]);
        }
    }
}