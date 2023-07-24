#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ROW_SIZE 5000  // maximum size of a row in CSV file
#define MAX_FIELD_SIZE 200  // maximum size of a field in CSV file

int main() {
    // declare the size from the pokemon file
    int numCols = 35; // display all columns
    int numRows = 152;
    int printCols = 2; // have a variable to just show 2 columns originally

    // open the CSV file
    FILE* fp = fopen("FirstGenPokemon.csv", "r");
    if (!fp) {
        printf("Can't open pokemon data file\n");
        return -1;
    }
    else {
        printf("FILE OPENED SUCCESSFULLY\n");
    }

    // allocate memory for the data
    char*** data = malloc(sizeof(char**) * numRows);
    for (int i = 0; i < numRows; i++) {
        data[i] = malloc(sizeof(char*) * numCols);
        for (int j = 0; j < numCols; j++)
            data[i][j] = malloc(sizeof(char) * MAX_FIELD_SIZE);
    }

    // read the CSV file
    char line[MAX_ROW_SIZE];
    int row_count = 0;
    while (fgets(line, sizeof(line), fp)) {
        char* tok = strtok(line, ",");
        for (int field_count = 0; tok && field_count < numCols; field_count++) { // making sure the pointer to the string in the file and the index for the array's column are inbounds
            strcpy(data[row_count][field_count], tok); // copy the line item from the .csv into the program's memory
            tok = strtok(NULL, ","); // increment tok to next line item
        }
        row_count++;
    }
    fclose(fp); // close file connection since it has been read


    // display the data
    for (int i = 0; i < numRows; i++) {
        printf("No. %s: %s\n", data[i][0], data[i][1]);
    }

    // ask user for a pokemon to display
    printf("Choose a pokemon to see details: ");
    int choice;
    scanf("%d", &choice); // read for an integer input
    // check if choice is valid
    if(choice < 1 || choice > numRows) {
        printf("Your choice is invalid.\n");
    }
    else {
        printf(" ");
        for(int p = 0; p < numCols; p++) { // go through each column
            // print the header from row 0 and then the data from row (choice) and filter with if
            if(((0 <= p) && (p <= 6)) || (p == 9) || (11 <= p) && (p <= 32)) {
                if((p == 5) || (p == 12) || (p == 18)) {
                    // break up the long text block
                    printf("\n");
                }
                //printf("(p = %d)", p);
                printf("%s: %s \n", data[0][p], data[choice][p]);
            }
        }
    }
    // free the allocated memory
    for (int i = 0; i < numRows; i++) {
        for (int j = 0; j < numCols; j++)
            free(data[i][j]);
        free(data[i]);
    }
    free(data);

    return 0;
}
