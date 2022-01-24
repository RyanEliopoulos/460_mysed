

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void sed(char *, char *, FILE *);
int find(char [], char [], size_t *);
FILE *open_file(char *);
void replace(char [], char [], size_t);


int main(int argc, char *argv[]) {

    if (argc < 3) {
        printf("my-sed: find term replace term [file ...]\n");
        exit(1);
    }
    // At least 2 arguments provided
    char *find_term = argv[1];
    char *replace_term = argv[2];

    // Replacing terms
    if (argc == 3) {  // No input files provided
        sed(find_term, replace_term, stdin);
    }
    else {  // Iterating through provided filepaths
        for (int i = 3; i < argc; i++) {
            FILE *file = open_file(argv[i]);   // error handling handled inside function
            sed(find_term, replace_term, file);
            fclose(file);
        }
    }

    return 0;
}

   
FILE *open_file(char *filepath) {
    // Wrapper for fopen. Adds error handling.  
    
    FILE *file = fopen(filepath, "r");
    if (file == NULL) {
        printf("my-sed: cannot open file '%s'\n", filepath);
        exit(1);
    }
    return file;
}


void sed(char *find_term, char *replace_term, FILE *file) {
    // Finds first instance of 'find_term' for each line in the filestream 'file' 
    // and replaces it with 'replace_term'
    
    // getline variables
    char *lineptr = NULL;     
    size_t n = 0;
    size_t linesize = 0; 
    // other variables  
    size_t match_index = 0;  // Updated by find if the target string is found within the input string

    while ((linesize = getline(&lineptr, &n, file)) != -1) {
        int found = find(find_term, lineptr, &match_index);
        if (found) {
            replace(replace_term, lineptr, match_index);
        }
        printf("%s", lineptr);
    }
    free(lineptr);
}



int find(char find_term[], char line[], size_t *beginning_index) {
    // Attempts to find the index of string 'line' where the substring 'find_term' begins.
    // Updates beginning_index if it exists and returns 1, otherwise returns 0

    // Establishing length for loop backstop
    size_t find_len = strlen(find_term); 
    size_t line_len = strlen(line);

    // size_t arithmetic doesn't work if the result is negative. 
    // Guard in place to compensate for this effect in the outer for loop.
    if (find_len > line_len) return 0;   

    for (size_t i = 0; i < (line_len - find_len); i++) {  
        for (size_t j = 0; ; j++) {
            if (j >= find_len) {  
                // Entire find_term has been found within 'line'
                *beginning_index = i;
                return 1;
            }
            if (find_term[j] != line[i + j]) break;
        }
    }
    return 0;
}

void replace(char replace_term[], char line[], size_t index) {
    size_t i = 0;
    while (replace_term[i] != '\0') {
        line[i + index] = replace_term[i];
        i++;
    }
}
