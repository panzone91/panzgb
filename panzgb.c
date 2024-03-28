#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "panzgb.h"

#define NUM_OP_60HZ (GB_CLOCK / GB_SCREEN_REFRESH_RATE)

#define SCALE 4

void doScreenshoot(SDL_Renderer *renderer) {
    time_t name;
    name = time(NULL);
    SDL_Surface *sshot =
        SDL_CreateRGBSurface(0, 160 * SCALE, 144 * SCALE, 32, 0x00ff0000,
                             0x0000ff00, 0x000000ff, 0xff000000);
    SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ARGB8888,
                         sshot->pixels, sshot->pitch);
    SDL_SaveBMP(sshot, ctime(&name));
    SDL_FreeSurface(sshot);
}

void renderScreen(gb *cpu, SDL_Renderer *rend) {
    int x, y;
    int j;
    SDL_Rect rectangle;
    rectangle.h = SCALE;
    SDL_SetRenderDrawColor(rend, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(rend);
    for (y = 0; y < 144; y++) {
        for (x = 0; x < 160; x++) {
            BYTE color = getPixelColor(cpu, x, y);
            for (j = x + 1; color == getPixelColor(cpu, j, y) && j < 160; j++)
                ;
            rectangle.w = SCALE * (j - x);
            rectangle.x = x * SCALE;
            rectangle.y = y * SCALE;
            x = j - 1;
            SDL_SetRenderDrawColor(rend, color, color, color, SDL_ALPHA_OPAQUE);
            SDL_RenderFillRect(rend, &rectangle);
        }
    }
    SDL_RenderPresent(rend);
}

void getInput(gb *cpu, SDL_Renderer *rend) {
    SDL_Event event;
    SIGNED_BYTE key = 0;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            exit(EXIT_SUCCESS);
        }
        if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
            switch (event.key.keysym.sym) {
            case SDLK_z:
                key = GB_K_A;
                break;
            case SDLK_x:
                key = GB_K_B;
                break;
            case SDLK_RETURN:
                key = GB_K_START;
                break;
            case SDLK_SPACE:
                key = GB_K_SELECT;
                break;
            case SDLK_UP:
                key = GB_K_UP;
                break;
            case SDLK_LEFT:
                key = GB_K_LEFT;
                break;
            case SDLK_RIGHT:
                key = GB_K_RIGHT;
                break;
            case SDLK_DOWN:
                key = GB_K_DOWN;
                break;

            case SDLK_f:
                doScreenshoot(rend);
                return;

            default:
                return;
            }
            if (event.type == SDL_KEYUP) {
                changeKeyState(cpu, key, GB_KEY_RELEASED);
            } else {
                changeKeyState(cpu, key, GB_KEY_PRESSED);
            }
        }
    }
}

gb *gameboy;

int main(int argc, char **argv) {
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *window = SDL_CreateWindow("panz-gb", SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED, 160 * SCALE,
                                          144 * SCALE, 0);
    SDL_Renderer *renderer =
        SDL_CreateRenderer(window, -1, 0);

    SDL_AudioSpec want, obtained;

    want.freq = 44100;
    want.format = AUDIO_U8;
    want.channels = 2;
    want.samples = 2048;
    want.callback = NULL;

    SDL_AudioDeviceID audioDeviceId = SDL_OpenAudioDevice(NULL, 0, &want, &obtained, 0);
    SDL_PauseAudioDevice(audioDeviceId, 0);

    gameboy = newGameboy(argv[1]);
    if (!gameboy) {
        fprintf(stderr, "Error on memory allocation\n");
        exit(EXIT_FAILURE);
    }

    unsigned int numOperation = 0;
    while (1) {
        unsigned int timeStartFrame = SDL_GetTicks();
        getInput(gameboy, renderer);
        while (numOperation < NUM_OP_60HZ)
            numOperation += executeGameBoy(gameboy);
        numOperation -= NUM_OP_60HZ;
        renderScreen(gameboy, renderer);

        unsigned int soundDataSize = 0;
        BYTE* soundData = getSoundData(gameboy,&soundDataSize);

        if(soundDataSize != 0) {
            SDL_QueueAudio(audioDeviceId, soundData, soundDataSize);
        }

        float deltaT =
            (float)1000 / (59.7) - (float)(SDL_GetTicks() - timeStartFrame);
        if (deltaT > 0)
            SDL_Delay((unsigned int)deltaT);
    }
}
