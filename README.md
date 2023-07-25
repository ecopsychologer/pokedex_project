# Another C Pokedex Project!
I'm working on a pokedex in c to make use of a 2" screen I have. I'm hoping that this application will, in time, be able to take a .CSV file input to populate a pokemon data array, then allow the user to scroll a list of these pokemon and click on one to see more information.
As is mentioned at the top of pokedex_SDL.c, I built this with 
```
gcc -o pokedex pokedex_SDL.c -lSDL2 -lSDL2_ttf
```
Which creates a `pokedex` executable that can be run from the command line with
```
./pokedex
```
To begin with, I am only including the pokemon from this file: https://www.kaggle.com/datasets/dizzypanda/gen-1-pokemon

Also I found a CC0 font that I enjoy, which is in this repo.

The `pokedex.c` is what compiles into the `CLI_Kanto_Pokedex` which is run with `./CLI_Kanto_Pokedex` and compiled with 
```
gcc -o CLI_Kanto_Pokedex pokedex.c
```
