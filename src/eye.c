#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <stdio.h>

char *parse_args_get_filename(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: eye FILENAME\n");
        exit(2);
    }
    char *filename = argv[1];

    return filename;
}

int main(int argc, char *argv[])
{
    SDL_Window *window;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL couldn't initialise: %s.\n", SDL_GetError());
        exit(1);
    }

    char *filename = parse_args_get_filename(argc, argv);

    window = SDL_CreateWindow(
            "Image",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            800,
            800,
            SDL_WINDOW_BORDERLESS);

    int view_x = 800;
    int view_y = 800;

    //SDL_Surface* screen_surface = SDL_GetWindowSurface(window);

    if ( !IMG_Init(IMG_INIT_PNG) ) {
        fprintf(stderr, "Couldn't initialise image library.\n");
        exit(1);
    }

    //char* filename = "../data/projection1.png";
    //SDL_Surface* loadedSurface = IMG_Load(filename);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

    if ( renderer == NULL ) {
        fprintf(stderr, "Failed to create renderer: %s\n", SDL_GetError());
        exit(1);
    }

    SDL_Texture* imgTexture = IMG_LoadTexture(renderer, filename);

    Uint32 format;
    int access, w, h;
    SDL_QueryTexture(imgTexture, &format, &access, &w, &h);

    float image_aspect_ratio = (float) w / (float) h;

    printf("Loaded image is %dx%d, ar: %f\n", w, h, image_aspect_ratio);

    if ( imgTexture == NULL ) {
        fprintf(stderr, "Failed to load image as texture\n");
        exit(1);
    }


    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_RenderSetLogicalSize(renderer, 500, 500);

    int rect_w = (int) (image_aspect_ratio * 500.);
    SDL_Rect dstrect = {0, 0, rect_w, 500};

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, imgTexture, NULL, &dstrect);
    SDL_RenderPresent(renderer);

    int done = 0;

    SDL_Event e;
    while (!done) {
      while (SDL_PollEvent(&e)) {
	if (e.type == SDL_KEYDOWN) {
	  done = 1;
	}
      }
    }

    return 0;
}
