// rename replace_func to sed


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void replace_func(char *, char *, FILE *);
int find(char [], char [], size_t *);
FILE *open_file(char *);


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
        printf("Reading from stdin\n");
        replace_func(find_term, replace_term, stdin);
    }
    else {  // Iterating through provided filepaths
        for (int i = 3; i < argc; i++) {
            FILE *file = open_file(argv[i]);   // error handling handled inside function
            replace_func(find_term, replace_term, file);
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


void replace_func(char *find_term, char *replace_term, FILE *file) {
    // Finds first instance of 'find_term' for each line in the filestream 'file' 
    // and replaces it with 'replace_term'
    
    // getline variables
    char *lineptr = NULL;     
    size_t n = 0;
    size_t linesize = 0; 
    // other variables  
    size_t match_index = 0;  // Updated by find if the target string is found within the input string

    while ((linesize = getline(&lineptr, &n, file)) != -1) {
        int ret = find(find_term, lineptr, &match_index);
        if (!ret) {
            //printf("term: %s\n", find_term); 
            //printf("match index: %lu\n", match_index);
            printf("%s", lineptr); 
        }
        //if (ret) replace(replace_term, match_index, lineptr);
    }
    free(lineptr);
}



int find(char find_term[], char line[], size_t *beginning_index) {
    // Attempts to find the index of string 'line' where the substring 'find_term' begins.
    // Updates beginning_index if it exists and returns 0, otherwise returns -1

    // Establishing length for loop backstop
    size_t find_len = strlen(find_term); 
    //printf("findlen: %lu\n", find_len);
    size_t line_len = strlen(line);
    //printf("line_len: %lu\n", line_len);

    // size_t arithmetic doesn't work if the result is negative. 
    // Guard in place to compensate for this effect in the outer for loop.
    if (find_len > line_len) return -1;   

    for (size_t i = 0; i < (line_len - find_len); i++) {  
        for (size_t j = 0; ; j++) {
            if (j >= find_len) {  
                // Entire find_term has been found within 'line'
                *beginning_index = i;
                return 0;
            }
            if (find_term[j] != line[i + j]) break;
        }
    }
    return -1;
}
