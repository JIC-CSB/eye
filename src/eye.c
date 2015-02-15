#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <stdio.h>

int main(int argc, char *argv[])
{
    SDL_Window *window;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL couldn't initialise: %s.\n", SDL_GetError());
        exit(1);
    }

    // const SDL_VideoInfo *info = SDL_GetVideoInfo();

    // int display_x = info->current_w;
    // int display_y = info->current_h;

    // printf("%dx%d\n", display_x, display_y);

    window = SDL_CreateWindow(
            "Image",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            400,
            600,
            SDL_WINDOW_BORDERLESS);

    //SDL_Surface* screen_surface = SDL_GetWindowSurface(window);

    if ( !IMG_Init(IMG_INIT_PNG) ) {
        fprintf(stderr, "Couldn't initialise image library.\n");
        exit(1);
    }

    char* filename = "../data/projection1.png";
    //SDL_Surface* loadedSurface = IMG_Load(filename);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

    if ( renderer == NULL ) {
        fprintf(stderr, "Failed to create renderer: %s\n", SDL_GetError());
        exit(1);
    }

    SDL_Texture* imgTexture = IMG_LoadTexture(renderer, filename);

    if ( imgTexture == NULL ) {
        fprintf(stderr, "Failed to load image as texture\n");
        exit(1);
    }

    // if ( loadedSurface == NULL ) {
    //     fprintf(stderr, "Couldn't load image.\n");
    //     exit(1);
    // }

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, imgTexture, NULL, NULL);
    SDL_RenderPresent(renderer);
    //SDL_Surface *optimised_surface = SDL_ConvertSurface(loadedSurface, screen_surface->format, 0);

    //SDL_BlitSurface(optimised_surface, NULL, screen_surface, NULL);

    //SDL_UpdateWindowSurface(window);

    SDL_Delay(2000);
    return 0;
}
