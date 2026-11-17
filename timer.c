#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>

#define WIDTH 200
#define HEIGHT 120
#define FONT_SIZE 52
#define TOTAL_SECONDS 45 * 60 // 45 MIN

SDL_Color textColor = {101, 67, 33, 255};
SDL_Color bgColor = {245, 245, 220, 255};
SDL_Color pausedTextColor = {115, 115, 115, 255};
SDL_Color pausedBgColor = {200, 200, 200, 255};
SDL_Color finishedTextColor = {255, 255, 255, 255};
SDL_Color finishedBgColor = {70, 150, 90, 255};

TTF_Font *load_font(const char *filename, int size) {
  TTF_Font *font = TTF_OpenFont(filename, size);
  if (font) {
    return font;
  }

  const char *home = getenv("HOME");
  if (home) {
    char path[256];
    snprintf(path, sizeof(path), "%s/.local/share/fonts/%s", home, filename);

    font = TTF_OpenFont(path, size);
    if (font) {
      return font;
    }
  }

  fprintf(stderr, "Error loading font: %s\n", TTF_GetError());
  return NULL;
}

void render_time(SDL_Renderer *renderer, TTF_Font *font, int seconds,
                 SDL_Color color) {
  char time_str[16];
  int minutes = seconds / 60;
  seconds %= 60;
  snprintf(time_str, sizeof(time_str), "%02d:%02d", minutes, seconds);

  SDL_Surface *surface = TTF_RenderText_Blended(font, time_str, color);
  if (!surface) {
    fprintf(stderr, "Error rendering timer text: %s\n", TTF_GetError());
    return;
  }

  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (!texture) {
    fprintf(stderr, "Error creating timer text texture: %s\n", SDL_GetError());
    SDL_FreeSurface(surface);
    return;
  }

  SDL_Rect rect = {(WIDTH - surface->w) / 2, (HEIGHT - surface->h) / 2,
                   surface->w, surface->h};

  SDL_RenderCopy(renderer, texture, NULL, &rect);
  SDL_FreeSurface(surface);
  SDL_DestroyTexture(texture);
}

int main(void) {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
    fprintf(stderr, "Error initializing SDL: %s\n", SDL_GetError());
    return 1;
  }

  if (TTF_Init() != 0) {
    fprintf(stderr, "Error initializing SDL_ttf: %s\n", TTF_GetError());
    SDL_Quit();
    return 1;
  }

  TTF_Font *font = TTF_OpenFont("JetBrainsMono-Medium.ttf", FONT_SIZE);
  if (!font) {
    font = load_font("JetBrainsMono-Medium.ttf", FONT_SIZE);
  }

  if (!font) {
    TTF_Quit();
    SDL_Quit();
    return 1;
  }

  SDL_Window *window =
      SDL_CreateWindow("Study Timer", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
  if (!window) {
    fprintf(stderr, "Error creating window: %s\n", SDL_GetError());
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
    return 1;
  }

  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer) {
    fprintf(stderr, "Error creating renderer: %s\n", SDL_GetError());
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
    return 1;
  }

  Uint32 startTime = SDL_GetTicks();
  int isPaused = 0;
  Uint32 pausedElapsed = 0;

  int running = 1;
  SDL_Event event;

  while (running) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT)
        running = 0;
      if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (isPaused) {
          // reset the time to what it was before pausing
          startTime = SDL_GetTicks() - pausedElapsed;
          isPaused = 0;
        } else {
          pausedElapsed = SDL_GetTicks() - startTime;
          isPaused = 1;
        }
      }
    }

    Uint32 currentTime = SDL_GetTicks();
    int elapsed;

    if (isPaused) {
      elapsed = pausedElapsed / 1000;
    } else {
      elapsed = (currentTime - startTime) / 1000;
    }

    int remaining = TOTAL_SECONDS - elapsed;

    SDL_Color currentBgColor;
    SDL_Color currentTextColor;
    if (remaining <= 0) {
      currentBgColor = finishedBgColor;
      currentTextColor = finishedTextColor;
    } else if (isPaused) {
      currentBgColor = pausedBgColor;
      currentTextColor = pausedTextColor;
    } else {
      currentBgColor = bgColor;
      currentTextColor = textColor;
    }

    SDL_SetRenderDrawColor(renderer, currentBgColor.r, currentBgColor.g,
                           currentBgColor.b, currentBgColor.a);
    SDL_RenderClear(renderer);

    render_time(renderer, font, remaining > 0 ? remaining : 0,
                currentTextColor);

    SDL_RenderPresent(renderer);
    SDL_Delay(1000); // 1fps, no need to render faster
  }

  TTF_CloseFont(font);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  TTF_Quit();
  SDL_Quit();
  return 0;
}
