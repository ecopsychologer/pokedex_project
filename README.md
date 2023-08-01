# Another Pokedex Project!
I'm working on a Pokedex in C to (eventually) make use of a 2" screen I have. This program uses a CSV file with Pokemon data as well as a folder with resources constructed using `gif_downloader.c` and `image_downloader.c` and then processes it into a scrollable list of pokemon.

The list interface is navigated with the `Up` and `Down` arrow keys, which can be selected to view more details of a single pokemon with `Enter`, and you can escape sinle pokemon view with `Esc`. The `Left` and `Right` keys switch pages within the single pokemon view.

As is mentioned at the top of pokedex_SDL.c, I built this with 
```
gcc -o pokedex pokedex_SDL.c -lSDL2 -lSDL2_ttf -lSDL2_image
```
Which creates a `pokedex` executable that can be run from the command line with
```
./pokedex
```
Right now, I am only including the pokemon from this file: https://www.kaggle.com/datasets/dizzypanda/gen-1-pokemon

Also I found a CC0 font that I enjoy, which is in this repo.

The `pokedex.c` is what compiles into the `CLI_Kanto_Pokedex` which is run with `./CLI_Kanto_Pokedex` and compiled with 
```
gcc -o CLI_Kanto_Pokedex pokedex.c
```
 * To do list:
 * 
 * ~ Now
 * - change font to black
 * - add white rectangles as text boxes behind text
 * - add one time animation to single pokemon view
 * - add consistent animation to the icon of the pokemon in the list view
 * 
 * ~ Later
 * - add a pokeball icon instead of the pokemon icon when scrolling
 * - expand to include all gen 3 pokemon
 * - begin to test hardware interaction
