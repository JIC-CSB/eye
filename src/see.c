#include <stdio.h>

#include <FreeImage.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

char *parse_args_get_filename(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: a.out FILENAME\n");
        exit(2);
    }
    char *filename = argv[1];

    return filename;
}


int main(int argc, char *argv[]) {

  char *filename = parse_args_get_filename(argc, argv);

  FREE_IMAGE_FORMAT filetype = FreeImage_GetFileType(filename, 0);
  printf("File type              : %d\n", filetype);

  FIBITMAP *dib = FreeImage_Load(filetype, filename, 0);

  printf("Bits per pixel in input: %d\n", FreeImage_GetBPP(dib));
  if (FreeImage_GetColorType(dib) == FIC_MINISBLACK) {
    // Single channel so ensure image dib is compressed to 8-bit.
    FIBITMAP *tmp = FreeImage_ConvertToGreyscale(dib);
    FreeImage_Unload(dib);
    dib = tmp;
    printf("Bits per pixel used    : %d\n", FreeImage_GetBPP(dib));
  }
  
  FreeImage_FlipVertical(dib);  // Loaded image is upside down, so flip it.
  
  unsigned width = FreeImage_GetWidth(dib);
  unsigned height = FreeImage_GetHeight(dib);
  printf("Width                  : %d\n", width);
  printf("Height                 : %d\n", height);

  SDL_Surface *screen_surface = SDL_CreateRGBSurfaceFrom(
    FreeImage_GetBits(dib),
    width,
    height,
    FreeImage_GetBPP(dib),
    FreeImage_GetPitch(dib),
    FreeImage_GetRedMask(dib),
    FreeImage_GetGreenMask(dib),
    FreeImage_GetBlueMask(dib),
    0 
  );


  if (FreeImage_GetBPP(dib) == 8) {
    // We are dealing with a grey scale image so we need to create an
    // appropriate palette.
    SDL_Color colors[256];
    int i;

    for(i = 0; i < 256; i++) {
      colors[i].r = colors[i].g = colors[i].b = i;
    }

    SDL_SetPaletteColors(screen_surface->format->palette, colors, 0, 256);
  }


  if (screen_surface == NULL) {
    fprintf(stderr, "Failed to create surface: %s\n", SDL_GetError());
    exit(1);
  }

  // Display the image using SDL2.
  SDL_Window *window;
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    fprintf(stderr, "SDL couldn't initialise: %s.\n", SDL_GetError());
    exit(1);
  }

  window = SDL_CreateWindow( "Image",
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    width,
    height,
    SDL_WINDOW_BORDERLESS);

  if ( !IMG_Init(IMG_INIT_PNG) ) {
    fprintf(stderr, "Couldn't initialise image library.\n");
    exit(1);
  }

  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
  if ( renderer == NULL ) {
    fprintf(stderr, "Failed to create renderer: %s\n", SDL_GetError());
    exit(1);
  }

//  SDL_Texture* imgTexture = IMG_LoadTexture(renderer, filename);
  SDL_Texture* imgTexture = SDL_CreateTextureFromSurface(renderer, screen_surface);

  if ( imgTexture == NULL ) {
    fprintf(stderr, "Failed to load image as texture\n");
    exit(1);
  }

  SDL_Rect dstrect = {0, 0, width, height};
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

  FreeImage_Unload(dib);
  SDL_FreeSurface(screen_surface);
  return 0;
}

