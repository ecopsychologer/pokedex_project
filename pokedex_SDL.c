/**
 * 
 * Build: gcc -o pokedex pokedex_SDL.c -lSDL2 -lSDL2_ttf
 * 
**/

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_ROW_SIZE 5000
#define MAX_FIELD_SIZE 400
#define NUM_ROWS 152
#define NUM_COLS 35
#define MAX_DISPLAY_ROWS 20
#define TARGET_FRAME_RATE 60
#define POKEDEX_ENTRY_SIZE 20000
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define FONT_SIZE 24

typedef struct {
    SDL_Surface* surface;
    SDL_Texture* texture;
} RenderedText;

/*
 *
 * Loading Data
 * 
 */

char*** load_data() {
    // open the CSV file
    FILE* fp = fopen("FirstGenPokemonV2.csv", "r");
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

/*
 *
 * SDL Rendering into an Array
 * 
 */

RenderedText* load_rendered_text(char*** data, TTF_Font* font, SDL_Color color, SDL_Renderer* renderer) {
    // memory allocation for the text to the size of the rendered text (x) by the number of rows (y)
    RenderedText* rendered_text = malloc(sizeof(RenderedText) * NUM_ROWS);

    for (int i = 0; i < NUM_ROWS; i++) {
        // make a string for current pokemon
        char pokemon_string[MAX_FIELD_SIZE];
        if (i == 0) {
            sprintf(pokemon_string, " ID No. - Name");
        } else {
            sprintf(pokemon_string, " * No. %s: %s", data[i][0], data[i][1]);
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

/*
 * 
 * Render Text
 * 
 * This function takes in a string,
 * a font, a color, and a renderer,
 * and creates a surface and a
 * texture for that text string. It
 * returns a 'RenderedText' struct that
 * contains the surface and the
 * texture.
 * 
 */
RenderedText render_text(char* string, TTF_Font* font, SDL_Color color, SDL_Renderer* renderer) {

    RenderedText render_text;

    // create a surface
    render_text.surface = TTF_RenderText_Solid(font, string, color);
    if (render_text.surface == NULL) {
        printf("Text rendering failed, error: %s\n", TTF_GetError());
    }

    // create a texture
    render_text.texture = SDL_CreateTextureFromSurface(renderer, render_text.surface);
    if (render_text.texture == NULL ) {
        printf("Failed to create texture : %s\n", SDL_GetError());
    }

    return render_text;
}

/*
 * 
 * Splits strings into lines
 * 
 */
char** split_string_into_lines(char* string, int* line_count) {
    // count how many lines we have
    int count = 1; // at least 1 line, presumably
    for (int i = 0; string[i]; i++) {
        if (string[i] == '\n') {
            count++;
        }
    }

    // allocate memory for the lines
    // lines the 2D array to hold strings
    char** lines = malloc(sizeof(char*) * count);

    // make a copy of the original string before tokenizing
    char* str = strdup(string);

    // now split the string into individual lines, by delimiter \n
    char* line = strtok(str, "\n");
    for (int i = 0; i < count; i++) {
        if (line == NULL) { // if line is null, just fill with a space
            lines[i] = strdup(" ");
            line = strtok(NULL, "\n");
        } else {
            lines[i] = strdup(line); // ith position in array gets the first line of chars
            line = strtok(NULL, "\n"); // "increment" line
        }
    }

    free(str);

    *line_count = count;
    return lines;
}


/*
 *
 * Main Function
 * 
 */
int main(int argc, char* argv[]) {
    // create an int to track our position in list
    int scroll_offset = 0;
    // cursor position tracker - starts at pos 1 because the file has a header row
    int cursor_position = 1;
    // selected position, -1 means no selection yet
    int selected_position = -1;
    // variable to track whether we're viewing one pokemon or not
    bool single_pokemon_view = false;

    // Load Pokemon data
    char*** data = load_data();

    // Initialize SDL
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    // Create a window
    SDL_Window *window = SDL_CreateWindow("Pokedex", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);

    // Create a renderer (this is what we'll draw to)
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

    // Create a font
    TTF_Font* font = TTF_OpenFont("PixelOperatorHB8.ttf", FONT_SIZE);
    if (font == NULL) {
        printf("Failed to load font: %s\n", TTF_GetError());
        exit(1);
    }

    // Create a color for our text
    SDL_Color color = {255, 255, 255, 255};
    // and a highlight color
    SDL_Color highlight_color = {55, 55, 220, 255};

    RenderedText* rendered_text = load_rendered_text(data, font, color, renderer);

    // create a rectangular surface and texture for highlighting
    SDL_Surface* highlight_surface = SDL_CreateRGBSurface(0,800,30,32,0,0,0,0);
    // fill this rectangle with blue
    SDL_FillRect(highlight_surface, NULL, SDL_MapRGB(highlight_surface->format,25,55,180));
    // create the texture
    SDL_Texture* highlight_texture = SDL_CreateTextureFromSurface(renderer, highlight_surface);

    // infinite for loop to handle drawing of the surface/screen
    for (;;) {

        /*
         *
         * Event Loop
         * 
         */

        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                // Quit the application
                goto done;
            } else if (event.type == SDL_KEYDOWN) {
                // up arrow
                if (event.key.keysym.sym == SDLK_UP) {
                    // scroll up
                    // make sure the cursor stays off the header
                    if (cursor_position > 1) { 
                        cursor_position--;
                        if (selected_position > 1) {
                            selected_position--;
                        }
                    }
                    if (scroll_offset > 0 && cursor_position < scroll_offset + MAX_DISPLAY_ROWS / 2) {
                        scroll_offset--;
                    }
                } 
                // down arrow
                else if (event.key.keysym.sym == SDLK_DOWN) {
                    // scroll down
                    if (cursor_position < NUM_ROWS - 1) {
                        cursor_position++; // stay within the list
                        if (selected_position < NUM_ROWS - 1) {
                            selected_position++;
                        }
                    }
                    if (scroll_offset < NUM_ROWS - MAX_DISPLAY_ROWS && cursor_position > scroll_offset + MAX_DISPLAY_ROWS / 2) scroll_offset++;
                } 
                // enter/return
                else if (event.key.keysym.sym == SDLK_RETURN) {
                    // user selected current row
                    selected_position = cursor_position;
                    single_pokemon_view = true;
                }
                // escape
                else if (event.key.keysym.sym == SDLK_ESCAPE) {
                    // user wants to go back
                    single_pokemon_view = false;
                }
            }
        }

        // Clear the screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

         /*
          *
          * render loop to render the pokemon
          * names and ID's
          * 
          */

        for(int i = scroll_offset; i < scroll_offset + MAX_DISPLAY_ROWS && i < NUM_ROWS; i++) {

            // Single Pokemon View Rendering
            if (single_pokemon_view && selected_position >= 1 && selected_position < NUM_ROWS) {
                // display the selected pokemon's info
                char info_string[POKEDEX_ENTRY_SIZE] = "";
                sprintf(info_string, "Pokemon No. %s: %s\n", data[selected_position][0], data[selected_position][1]);
                for (int k = 2; k < NUM_COLS; k++) { // start at 2 to ignore numbers & names columns
                    // print the header from row 0 and then the data from row (choice) and filter with if
                    if(((0 <= k) && (k <= 6)) || (k == 9) || (11 <= k) && (k <= 32)) {
                        if((k == 5) || (k == 12) || (k == 18)) {
                            // break up the long text block
                            sprintf(info_string + strlen(info_string), "\n");
                        }
                        //printf("(p = %d)", p);
                        sprintf(info_string + strlen(info_string),"%s: %s \n", data[0][k], data[selected_position][k]);
                    }
                }

                // trying to do a little functional fix to make new lines work with SDL by typing '\n'
                int line_count;
                char** lines = split_string_into_lines(info_string, &line_count);

                // The actual rendering of the information from the selected pokemon's line
                for (int j = 0; j < line_count; j++) {
                    RenderedText info_text = render_text(lines[j], font, color, renderer);
                    SDL_Rect rect = {50, 50 + j * info_text.surface->h, info_text.surface->w, info_text.surface->h};
                    SDL_RenderCopy(renderer, info_text.texture, NULL, &rect);

                    SDL_FreeSurface(info_text.surface);
                    SDL_DestroyTexture(info_text.texture);
                }

                free(lines);

                // break out of the loop after rendering data
                break;
            } 
            // end of Single Pokemon View Rendering

            // Full Pokemon List View Rendering
            else {
                // render the list of pokemon
                if (i == cursor_position) {
                    SDL_Rect highlight_rect = {40, 10 + (i - scroll_offset)*30,720,30};
                    SDL_RenderCopy(renderer, highlight_texture, NULL, &highlight_rect);
                }
                SDL_Rect rect = {50, 10 + (i - scroll_offset)*30, rendered_text[i].surface->w, rendered_text[i].surface->h};
                // copy a portion of the texture to the current rendering target
                SDL_RenderCopy(renderer, rendered_text[i].texture, NULL, &rect);
            }
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
    SDL_FreeSurface(highlight_surface);
    SDL_DestroyTexture(highlight_texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    free_data(data);

    return 0;
}