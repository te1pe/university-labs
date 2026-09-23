#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define Max_alphabet 256
#define Max_pattern 100
#define Max_buffer 10000
#define MAXX(a, b) ((a) >= (b) ? (a) : (b))

void shift_table(const char *pattern, int len_pattern, int *shift_arr) {
    for (int i = 0; i < Max_alphabet; i++) {
        shift_arr[i] = len_pattern;
    }
    for (int i = 0; i < len_pattern - 1; i++) {
        if (pattern[i] != ' ') {  
            shift_arr[(unsigned char)pattern[i]] = MAXX(1, len_pattern - 1 - i);
        }
    }
}

void boyer_moore_algorithm(const char *text, const char *pattern, int *shift_arr, int len_text, int len_pattern, int *positions, int *pos_count) {
    int i = len_pattern - 1;  
    while (i < len_text) {
        int j = len_pattern - 1; 
        int k = i; 
        while (j >= 0 && text[k] == pattern[j]) {
            positions[(*pos_count)++] = k + 1;  
            k--;
            j--;
        }
        if (j < 0) {  
            i += len_pattern;  
        } else {
            positions[(*pos_count)++] = k + 1;  
            i += shift_arr[(unsigned char)text[i]];
        }
    }
}

int main() {
    FILE *in_file = fopen("in.txt", "r");
    if (!in_file) {
        return 0;
    }
    FILE *out_file = fopen("out.txt", "w");
    if (!out_file) {
        fclose(in_file);
        return 0;
    }
    char pattern[Max_pattern];  
    char *text = NULL;  
    char *temp_text = NULL;   
    size_t text_size = 0;
    char buffer[Max_buffer]; 
    int is_first_line = 1;
    while (fgets(buffer, sizeof(buffer), in_file)) {
        if (is_first_line) {
            strcpy(pattern, buffer);
            pattern[strcspn(pattern, "\n")] = '\0'; 
            is_first_line = 0;
        } else {
            buffer[strcspn(buffer, "\n")] = ' '; 
            size_t buffer_len = strlen(buffer);
            temp_text = realloc(text, text_size + buffer_len + 1);
            if (!temp_text) {
                free(text);
                fclose(in_file);
                fclose(out_file);
                return 0;
            }
            else{
            text=temp_text;
            }
            
            strcpy(text + text_size, buffer);
            text_size += buffer_len;
        }
    }
    fclose(in_file);
    while (text_size > 0 && text[text_size - 1] == ' ') {
        text[text_size - 1] = '\0';
        text_size--;
    }
    int len_pattern = strlen(pattern);
    int shift_arr[Max_alphabet] = {0};
    shift_table(pattern, len_pattern, shift_arr);
    int *positions = malloc(text_size * sizeof(int)); 
    if (!positions) {
        free(text);
        fclose(out_file);
        return 0;
    }
    int pos_count = 0;
    boyer_moore_algorithm(text, pattern, shift_arr, text_size, len_pattern, positions, &pos_count);
    for (int i = 0; i < pos_count; i++) {
        if (i > 0) {
            fprintf(out_file, " ");
        }
        fprintf(out_file, "%d", positions[i]);
    } 
    fclose(out_file);
    free(text);
    free(positions);
    return 0;
}

