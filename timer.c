#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <time.h>

#define WIDTH 320
#define HEIGHT 220
#define FONT_SIZE 80
#define TOTAL_SECONDS 45 * 60 // 45 MIN

SDL_Color textColor = {101, 67, 33, 255};
SDL_Color bgColor = {245, 245, 220, 255};
SDL_Color alertColor = {255, 0, 0, 255};

TTF_Font *load_font(const char *filename, int size)
{
    TTF_Font *font = TTF_OpenFont(filename, size);
    if (font)
    {
        return font;
    }

    const char *home = getenv("HOME");
    if (home)
    {
        char path[256];
        snprintf(path, sizeof(path), "%s/.local/share/fonts/%s", home, filename);

        font = TTF_OpenFont(path, size);
        if (font)
        {
            return font;
        }
    }

    printf("Error loading font: %s\n", TTF_GetError());
    return NULL;
}

void render_time(SDL_Renderer *renderer, TTF_Font *font, int seconds)
{
    char time_str[6];
    int minutes = seconds / 60;
    seconds %= 60;
    snprintf(time_str, sizeof(time_str), "%02d:%02d", minutes, seconds);

    SDL_Surface *surface = TTF_RenderText_Blended(font, time_str, textColor);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect rect = {
        (WIDTH - surface->w) / 2,
        (HEIGHT - surface->h) / 2,
        surface->w,
        surface->h};

    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    TTF_Init();

    SDL_Window *window = SDL_CreateWindow("Study Timer",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          WIDTH, HEIGHT, 0);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1,
                                                SDL_RENDERER_ACCELERATED);

    TTF_Font *font = TTF_OpenFont("JetBrainsMono-Medium.ttf", FONT_SIZE);
    if (!font)
    {
        font = load_font("JetBrainsMono-Medium.ttf", FONT_SIZE);
    }

    Uint32 startTime = SDL_GetTicks();
    int isPaused = 0;
    Uint32 pausedElapsed = 0;

    int running = 1;
    SDL_Event event;

    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                running = 0;
            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                if (isPaused)
                {
                    // reset the time to what it was before pausing
                    startTime = SDL_GetTicks() - pausedElapsed;
                    isPaused = 0;
                }
                else
                {
                    pausedElapsed = SDL_GetTicks() - startTime;
                    isPaused = 1;
                }
            }
        }

        Uint32 currentTime = SDL_GetTicks();
        int elapsed;

        if (isPaused)
        {
            elapsed = pausedElapsed / 1000;
        }
        else
        {
            elapsed = (currentTime - startTime) / 1000;
        }

        int remaining = TOTAL_SECONDS - elapsed;

        // Clear screen
        if (remaining > 0)
        {
            SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, 255);
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, alertColor.r, alertColor.g, alertColor.b, 255);
        }

        SDL_RenderClear(renderer);

        if (remaining > 0)
        {
            render_time(renderer, font, remaining);
        }

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
