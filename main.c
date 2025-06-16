#include <stdio.h>
#include <SDL.h>
#include "chip8.h"
#define DEBUG

#define RENDER_DELAY 500

#define PIXEL_SiZE 20
// these will define SDL_Window dimensions
#define WINDOW_HEIGHT (CHIP8_SCREEN_HEIGHT * PIXEL_SiZE)
#define WINDOW_WIDTH  (CHIP8_SCREEN_WIDTH  * PIXEL_SiZE)

static void initPixels(SDL_Rect pixels[][CHIP8_SCREEN_WIDTH], int pixel_size, int rows, int cols);
static void renderScreen(SDL_Renderer* renderer, SDL_Rect pixels[][CHIP8_SCREEN_WIDTH], BYTE vmem[][CHIP8_SCREEN_WIDTH]);
static void updateKeyState(SDL_KeyCode key, BYTE state);

int main(void)
{
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Event event;

    SDL_Rect pixels[CHIP8_SCREEN_HEIGHT][CHIP8_SCREEN_WIDTH];

    // debug only
#ifdef DEBUG
    char ROM_path[] = "../ROMs/MyROM";

    if (CHIP8_loadROM(ROM_path) != CORRECT_EXIT)
    {
        printf("Failed to load ROM at %s\n", ROM_path);
        exit(-1);
    }
#endif

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

    CHIP8_init();
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

    // main game loop
    while (is_running)
    {
        while (SDL_PollEvent(&event) != 0)
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    is_running = 0;
                    break;
                case SDL_KEYDOWN:
                    updateKeyState(event.key.keysym.sym, 1);
                    break;
                case SDL_KEYUP:
                    updateKeyState(event.key.keysym.sym, 0);
                    break;
                default: break;
            }
        }

        CHIP8_decodeOp();

        // render current vmem state
        renderScreen(renderer, pixels, vmem);

        // wait before drawing the next frame
        SDL_Delay(RENDER_DELAY);
        // display current renderer to the screen
        SDL_RenderPresent(renderer);
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

/// Updates the state of the key in keys array to the given state (Pressed - 1, Unpressed - 0)
/// @param key SDL_KeyCode type
/// @param state The state to set the key to in keys array
static void updateKeyState(SDL_KeyCode key, BYTE state) {
    switch (key) {
        case SDLK_0:
            keys[0] = state;
            break;
        case SDLK_1:
            keys[0x1] = state;
            break;
        case SDLK_2:
            keys[0x2] = state;
            break;
        case SDLK_3:
            keys[0x3] = state;
            break;
        case SDLK_4:
            keys[0x4] = state;
            break;
        case SDLK_5:
            keys[0x5] = state;
            break;
        case SDLK_6:
            keys[0x6] = state;
            break;
        case SDLK_7:
            keys[0x7] = state;
            break;
        case SDLK_8:
            keys[0x8] = state;
            break;
        case SDLK_9:
            keys[0x9] = state;
            break;
        case SDLK_a:
            keys[0xA] = state;
            break;
        case SDLK_b:
            keys[0xB] = state;
            break;
        case SDLK_c:
            keys[0xC] = state;
            break;
        case SDLK_d:
            keys[0xD] = state;
            break;
        case SDLK_e:
            keys[0xE] = state;
            break;
        case SDLK_f:
            keys[0xF] = state;
            break;
        default:
            break;
    }
}