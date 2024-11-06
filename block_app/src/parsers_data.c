#include <stdio.h>
#include <string.h>
#include "parsers_data.h"
#include "log.h"

// #define DATA_TXT_PATH "./data/data.txt"
// #define BLOCK_WEB_TXT_PATH "./data/block_web.txt"

#define DATA_TXT_PATH "../data/data.txt"
#define BLOCK_WEB_TXT_PATH "../data/block_web.txt"

#define INIT_NUMBER_STRUCT 10
#define NUMBER_STRUCT_INCREASE 2

char line[256];

website_block* read_block_web(const char *filename, int *line_count) {
    website_block *list_block_web = NULL;
    *line_count = 0;
    int number_struct = INIT_NUMBER_STRUCT;
    list_block_web = malloc(number_struct * sizeof(website_block));
    if (list_block_web == NULL) {
        perror("Unable to allocate memory");
        return NULL;
    }
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Unable to open file");
        free(list_block_web);
        return NULL;
    }
    while (fgets(line, sizeof(line), file)) {
        if (*line_count >= number_struct) {
            number_struct *= 2;
            list_block_web = realloc(list_block_web, number_struct * sizeof(website_block));
            if (list_block_web == NULL) {
                perror("Unable to allocate memory");
                fclose(file);
                return NULL;
            }
        }
        line[strcspn(line, "\n")] = '\0';
        char *token = strtok(line, ", ");
        if (token != NULL) {
            strncpy(list_block_web[*line_count].url, token, MAX_LENGTH);
        }
        token = strtok(NULL, " ");
        if (token != NULL) {
            strncpy(list_block_web[*line_count].start_day, token, MAX_LENGTH);

        } else {
            list_block_web[*line_count].start_day[0] = '\0';
        }
        token = strtok(NULL, ", ");
        if (token != NULL) {
            strncpy(list_block_web[*line_count].start_time, token, MAX_LENGTH);
        } else {
            list_block_web[*line_count].start_time[0] = '\0';
        }
        token = strtok(NULL, " ");
        if (token != NULL) {
            strncpy(list_block_web[*line_count].end_day, token, MAX_LENGTH);
        } else {
            list_block_web[*line_count].end_day[0] = '\0';
        }

        token = strtok(NULL, " ");
        if (token != NULL) {
            strncpy(list_block_web[*line_count].end_time, token, MAX_LENGTH);
        } else {
            list_block_web[*line_count].end_time[0] = '\0';
        }
        (*line_count)++;
    }
    fclose(file);
    return list_block_web;
}

website_info* read_data_file(const char *filename, int *entry_count) {
    website_info *list_web = NULL;
    *entry_count = 0;
    int number_struct = INIT_NUMBER_STRUCT;
    list_web = malloc(number_struct * sizeof(website_info));
    if (list_web == NULL) {
        perror("Unable to allocate memory");
        return NULL;
    }
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Unable to open file");
        free(list_web);
        return NULL;
    }
    char line[MAX_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        if (*entry_count >= number_struct) {
            number_struct *= 2;
            list_web = realloc(list_web, number_struct * sizeof(website_info));
            if (list_web == NULL) {
                perror("Unable to allocate memory");
                fclose(file);
                return NULL;
            }
        }
        sscanf(line, "TIME: %s DATE: %s", list_web[*entry_count].time, list_web[*entry_count].date);
        fgets(line, sizeof(line), file);
        sscanf(line, "Name: %s", list_web[*entry_count].url);
        fgets(line, sizeof(line), file);
        sscanf(line, "IPv4 Address: %s", list_web[*entry_count].ip);
        (*entry_count)++;
        fgets(line, sizeof(line), file);
    }
    fclose(file);
    return list_web;
}

web_block_info* read_web_block_info(const char *filename, int *count) {
    web_block_info *list = NULL;
    *count = 0;
    int number_struct = INIT_NUMBER_STRUCT;
    list = malloc(number_struct * sizeof(web_block_info));
    if (list == NULL) {
        perror("Unable to allocate memory");
        return NULL;
    }
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Unable to open file");
        free(list);
        return NULL;
    }
    char line[MAX_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        if (*count >= number_struct) {
            number_struct *= 2;
            list = realloc(list, number_struct * sizeof(web_block_info));
            if (list == NULL) {
                perror("Unable to allocate memory");
                fclose(file);
                return NULL;
            }
        }
        sscanf(line, "%[^,], %[^,], %[^,], %[^,], %[^,], %[^,\n]", 
               list[*count].url, 
               list[*count].ip, 
               list[*count].start_day, 
               list[*count].start_time, 
               list[*count].end_day, 
               list[*count].end_time);
        (*count)++;
    }
    fclose(file);
    return list;
}

check* read_check_list(const char *filename, int *count) {
    check *list = NULL;
    *count = 0;
    int number_struct = INIT_NUMBER_STRUCT;
    list = malloc(number_struct * sizeof(check));
    if (list == NULL) {
        perror("Unable to allocate memory");
        return NULL;
    }

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Unable to open file");
        free(list);
        return NULL;
    }

    char line[MAX_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        if (*count >= number_struct) {
            number_struct *= 2;
            list = realloc(list, number_struct * sizeof(check));
            if (list == NULL) {
                perror("Unable to allocate memory");
                fclose(file);
                return NULL;
            }
        }
        line[strcspn(line, "\n")] = '\0';
        sscanf(line, "%[^,], %ld, %ld",
               list[*count].url,
               &list[*count].start_time_block,
               &list[*count].end_time_block);

        (*count)++;
    }

    fclose(file);
    return list;
}


// ----------- check function -----------//
// start check_function

web_block_info* get_web_block_info(int *out_count) {
    int line_count = 0;
    website_block *list_block = read_block_web(BLOCK_WEB_TXT_PATH, &line_count);
    int entry_count = 0;
    website_info *list_info = read_data_file(DATA_TXT_PATH, &entry_count);
    if (list_block == NULL || list_info == NULL) {
        *out_count = 0;
        return NULL;
    }
    int result_capacity = 10;
    int result_count = 0;
    web_block_info *result_list = malloc(result_capacity * sizeof(web_block_info));
    if (result_list == NULL) {
        perror("Unable to allocate memory");
        free(list_block);
        free(list_info);
        *out_count = 0;
        return NULL;
    }
    for (int i = 0; i < line_count; i++) {
        for (int j = 0; j < entry_count; j++) {
            if (strcmp(list_block[i].url, list_info[j].url) == 0) {
                if (result_count >= result_capacity) {
                    result_capacity *= 2;
                    result_list = realloc(result_list, result_capacity * sizeof(web_block_info));
                    if (result_list == NULL) {
                        perror("Unable to allocate memory");
                        free(list_block);
                        free(list_info);
                        *out_count = 0;
                        return NULL;
                    }
                }
                strncpy(result_list[result_count].url, list_block[i].url, MAX_LENGTH);
                strncpy(result_list[result_count].ip, list_info[j].ip, MAX_LENGTH);
                strncpy(result_list[result_count].start_day, list_block[i].start_day, MAX_LENGTH);
                strncpy(result_list[result_count].start_time, list_block[i].start_time, MAX_LENGTH);
                strncpy(result_list[result_count].end_day, list_block[i].end_day, MAX_LENGTH);
                strncpy(result_list[result_count].end_time, list_block[i].end_time, MAX_LENGTH);
                result_count++;
            }
        }
    }
    free(list_block);
    free(list_info);
    *out_count = result_count;
    return result_list;
}


void printf_to_file(const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Unable to open file");
        return;
    }

    int result_count = 0;
    web_block_info *list = get_web_block_info(&result_count);
    if (list == NULL) {
        fprintf(stderr, "No data to write to file\n");
        fclose(file);
        return;
    }

    char printed_ips[result_count][MAX_LENGTH];
    char printed_start_times[result_count][MAX_LENGTH];
    char printed_end_times[result_count][MAX_LENGTH];
    int printed_count = 0;

    for (int i = 0; i < result_count; i++) {
        int already_printed = 0;
        for (int j = 0; j < printed_count; j++) {
            if (strcmp(printed_ips[j], list[i].ip) == 0 &&
                strcmp(printed_start_times[j], list[i].start_time) == 0 &&
                strcmp(printed_end_times[j], list[i].end_time) == 0) {
                already_printed = 1;
                break;
            }
        }
        if (!already_printed) {
            fprintf(file, "%s,%s,%s,%s,%s,%s\n", 
                list[i].url,
                list[i].ip, 
                list[i].start_day, 
                list[i].start_time, 
                list[i].end_day, 
                list[i].end_time);
            strncpy(printed_ips[printed_count], list[i].ip, MAX_LENGTH);
            strncpy(printed_start_times[printed_count], list[i].start_time, MAX_LENGTH);
            strncpy(printed_end_times[printed_count], list[i].end_time, MAX_LENGTH);
            printed_count++;
        }
    }

    fclose(file);
    free(list);
}

void printf_ip_and_time_to_console() {
    int result_count = 0;
    web_block_info *list = get_web_block_info(&result_count);
    if (list == NULL) {
        return;
    }
    char printed_ips[result_count][MAX_LENGTH];
    int printed_count = 0;
    for (int i = 0; i < result_count; i++) {
        int already_printed = 0;
        for (int j = 0; j < printed_count; j++) {
            if (strcmp(printed_ips[j], list[i].ip) == 0) {
                already_printed = 1;
                break;
            }
        }
        if (!already_printed) {
            printf("%s\n", list[i].url);
            printf("%s\n", list[i].ip);
            printf("%s\n", list[i].start_day);
            printf("%s\n", list[i].start_time);
            printf("%s\n", list[i].end_day);
            printf("%s\n", list[i].end_time);
            printf("\n");
            strncpy(printed_ips[printed_count], list[i].ip, MAX_LENGTH);
            printed_count++;
        }
    }
    free(list);
}

// end check function


