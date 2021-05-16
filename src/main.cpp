#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#define WIDTH 800
#define HEIGHT 600
typedef struct AppData{
    TTF_Font *font;
    SDL_Texture *other;
    SDL_Texture *video;
    SDL_Texture *code;
    SDL_Texture *image;
    SDL_Texture *exec;
    SDL_Texture *direct;
    SDL_Texture *phrase;
}AppData;

void initialize(SDL_Renderer *renderer, AppData *data);
void render(SDL_Renderer *renderer, AppData *data);

int main(int argc, char **argv)
{
    char *home = getenv("HOME");
    printf("HOME: %s\n", home);

    // initializing SDL as Video
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();

    // create window and renderer
    SDL_Renderer *renderer;
    SDL_Window *window;
    SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer);

    // initialize and perform rendering loop
    AppData data;
    initialize(renderer, &data);
    render(renderer, &data);
    SDL_Event event;
    SDL_WaitEvent(&event);
    while (event.type != SDL_QUIT)
    {
        //render(renderer);
        SDL_WaitEvent(&event);
    }

    // clean up
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_DestroyTexture(data.other);
    SDL_DestroyTexture(data.exec);
    SDL_DestroyTexture(data.direct);
    SDL_DestroyTexture(data.image);
    SDL_DestroyTexture(data.video);
    SDL_DestroyTexture(data.code);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();

    return 0;
}

void initialize(SDL_Renderer *renderer, AppData *data)
{
    // set color of background when erasing frame
    SDL_SetRenderDrawColor(renderer, 235, 235, 235, 255);

    data->font = TTF_OpenFont("resrc/OpenSans-Regular.ttf", 18);
    SDL_Color phrase_color = {0,0,0};
    SDL_Surface *text_surf = TTF_RenderText_Solid(data->font, "video", phrase_color);
    data->phrase = SDL_CreateTextureFromSurface(renderer,text_surf);
    SDL_FreeSurface(text_surf);


    SDL_Surface *surf = IMG_Load("resrc/other.png");
    SDL_Surface *surf1 = IMG_Load("resrc/direct.png");
    SDL_Surface *surf2 = IMG_Load("resrc/exec.png");
    SDL_Surface *surf3 = IMG_Load("resrc/generic.png");
    SDL_Surface *surf4 = IMG_Load("resrc/video.png");
    SDL_Surface *surf5 = IMG_Load("resrc/code.png");
    data->other = SDL_CreateTextureFromSurface(renderer,surf);
    data->direct = SDL_CreateTextureFromSurface(renderer,surf1);
    data->exec = SDL_CreateTextureFromSurface(renderer,surf2);
    data->image = SDL_CreateTextureFromSurface(renderer,surf3);
    data->video = SDL_CreateTextureFromSurface(renderer,surf4);
    data->code = SDL_CreateTextureFromSurface(renderer,surf5);
    SDL_FreeSurface(surf);
    SDL_FreeSurface(surf1);
    SDL_FreeSurface(surf2);
    SDL_FreeSurface(surf3);
    SDL_FreeSurface(surf4);
    SDL_FreeSurface(surf5);
}

void render(SDL_Renderer *renderer, AppData *data)
{
    // erase renderer content
    SDL_SetRenderDrawColor(renderer, 0, 235, 235, 255);
    SDL_RenderClear(renderer);
    
    SDL_Rect rect;
    rect.x = 10;
    rect.y = 10;
    rect.w = 50;
    rect.h = 50;

    SDL_RenderCopy(renderer, data->other, NULL, &rect);

    rect.x = 10;
    rect.y = 60;

    SDL_RenderCopy(renderer, data->direct, NULL, &rect);

    rect.x = 10;
    rect.y = 120;

    SDL_RenderCopy(renderer, data->exec, NULL, &rect);

    rect.x = 10;
    rect.y = 180;

    SDL_RenderCopy(renderer, data->image, NULL, &rect);

    rect.x = 10;
    rect.y = 240;

    SDL_RenderCopy(renderer, data->video, NULL, &rect);

    rect.x = 10;
    rect.y = 300;

    SDL_RenderCopy(renderer, data->code, NULL, &rect);

    rect.x = 60;
    rect.y = 10;
    SDL_QueryTexture(data->phrase, NULL,NULL, &(rect.w), &(rect.h));
    SDL_RenderCopy(renderer, data->phrase, NULL, &rect);
    // show rendered frame
    SDL_RenderPresent(renderer);
}

