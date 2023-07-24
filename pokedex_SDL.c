/**
 * 
 * Build: gcc -o pokedex pokedex.c -lSDL2 -lSDL2_ttf
 * 
**/

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ROW_SIZE 5000
#define MAX_FIELD_SIZE 200
#define NUM_ROWS 152
#define NUM_COLS 35
#define MAX_DISPLAY_ROWS 20
#define TARGET_FRAME_RATE 60

typedef struct {
    SDL_Surface* surface;
    SDL_Texture* texture;
} RenderedText;

char*** load_data() {
    // open the CSV file
    FILE* fp = fopen("FirstGenPokemon.csv", "r");
    if (!fp) {
        printf("Can't open pokemon data file\n");
        exit(-1);
    }

    // allocate memory for the data
    char*** data = malloc(sizeof(char**) * NUM_ROWS);
    for (int i = 0; i < NUM_ROWS; i++) {
        data[i] = malloc(sizeof(char*) * NUM_COLS);
        for (int j = 0; j < NUM_COLS; j++)
            data[i][j] = malloc(sizeof(char) * MAX_FIELD_SIZE);
    }

    // read the CSV file
    char line[MAX_ROW_SIZE];
    int row_count = 0;
    while (fgets(line, sizeof(line), fp)) {
        char* tok = strtok(line, ",");
        for (int field_count = 0; tok && field_count < NUM_COLS; field_count++) {
            strcpy(data[row_count][field_count], tok);
            tok = strtok(NULL, ",");
        }
        row_count++;
    }

    fclose(fp);
    return data;
}

void free_data(char*** data) {
    for (int i = 0; i < NUM_ROWS; i++) {
        for (int j = 0; j < NUM_COLS; j++)
            free(data[i][j]);
        free(data[i]);
    }
    free(data);
}

RenderedText* load_rendered_text(char*** data, TTF_Font* font, SDL_Color color, SDL_Renderer* renderer) {
    // memory allocation for the text to the size of the rendered text (x) by the number of rows (y)
    RenderedText* rendered_text = malloc(sizeof(RenderedText) * NUM_ROWS);

    for (int i = 0; i < NUM_ROWS; i++) {
        // make a string for current pokemon
        char pokemon_string[MAX_FIELD_SIZE];
        if (i == 0) {
            sprintf(pokemon_string, " ID No. - Name\n");
        } else {
            sprintf(pokemon_string, " * No. %s: %s \n", data[i][0], data[i][1]);
        }

        // create a surface in the rendered text array
        rendered_text[i].surface = TTF_RenderText_Solid(font, pokemon_string, color);
        if(rendered_text[i].surface == NULL) {
            printf("Text rendering failed, error: %s\n", TTF_GetError());
        }
        // create a texture in the rendered text array
        rendered_text[i].texture = SDL_CreateTextureFromSurface(renderer, rendered_text[i].surface);
        if(rendered_text[i].texture == NULL) {
            printf("Failed to create texture: %s\n", SDL_GetError());
        }
    }
    return rendered_text;
}

void free_rendered_text(RenderedText* rendered_text) {
    for (int i = 0; i < NUM_ROWS; i++) { // sweep thru all rows
        SDL_FreeSurface(rendered_text[i].surface); // free surface @ i
        SDL_DestroyTexture(rendered_text[i].texture); // a dramatic liberation of that texture from memory
    }
    free(rendered_text);
}

int main(int argc, char* argv[]) {
    // create an int to track our position in list
    int scroll_offset = 0;

    // Load Pokemon data
    char*** data = load_data();

    // Initialize SDL
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    // Create a window
    SDL_Window *window = SDL_CreateWindow("Pokedex", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, 0);

    // Create a renderer (this is what we'll draw to)
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

    // Create a font
    TTF_Font* font = TTF_OpenFont("SummerPixel.ttf", 24);
    if (font == NULL) {
        printf("Failed to load font: %s\n", TTF_GetError());
        exit(1);
    }

    // Create a color for our text
    SDL_Color color = {255, 255, 255, 255};

    RenderedText* rendered_text = load_rendered_text(data, font, color, renderer);

    // infinite for loop to handle drawing of the surface/screen
    for (;;) {
        // event loop here to handle events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                // Quit the application
                goto done;
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_UP) {
                    // scroll up
                    if (scroll_offset > 0) scroll_offset--;
                } else if (event.key.keysym.sym == SDLK_DOWN) {
                    // scroll down
                    if (scroll_offset < NUM_ROWS - MAX_DISPLAY_ROWS) scroll_offset++;
                }
            }
        }

        // Clear the screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // render loop to render the pokemon names and ID's
        for(int i = scroll_offset; i < scroll_offset + MAX_DISPLAY_ROWS && i < NUM_ROWS; i++) {
            SDL_Rect rect = {50, 10 + (i - scroll_offset)*30, rendered_text[i].surface->w, rendered_text[i].surface->h};
            SDL_RenderCopy(renderer, rendered_text[i].texture, NULL, &rect);
        }
        // update the screen with the rendering stored in memory that was just rendered
        SDL_RenderPresent(renderer);

        // delay to limit frame rate to 60 FPS
        SDL_Delay(1000 / TARGET_FRAME_RATE);
    }
done:
    // Clean up
    free_rendered_text(rendered_text);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    free_data(data);

    return 0;
}
