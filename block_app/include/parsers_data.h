#ifndef PARSERS_DATA_H
#define PARSERS_DATA_H

#define MAX_LENGTH 256
#define MAX_TIME_LENGTH 20

typedef struct {
    char url[MAX_LENGTH];
    char start_day[MAX_LENGTH];
    char start_time[MAX_LENGTH];
    char end_day[MAX_LENGTH];
    char end_time[MAX_LENGTH];
} website_block;


typedef struct {
    char time[MAX_LENGTH];
    char date[MAX_LENGTH];
    char url[MAX_LENGTH];
    char ip[MAX_LENGTH];
} website_info;

typedef struct {
    char url[MAX_LENGTH];
    char ip[MAX_LENGTH];
    char start_day[MAX_LENGTH];
    char start_time[MAX_LENGTH];
    char end_day[MAX_LENGTH];
    char end_time[MAX_LENGTH];

} web_block_info;

typedef struct {
    char url[MAX_LENGTH];
    long start_time_block;
    long end_time_block;

} check;

website_block* read_block_web(const char *filename, int *line_count);
web_block_info* read_web_block_info(const char *filename, int *count);
check* read_check_list(const char *filename, int *count);
void printf_to_file(const char *filename);
void printf_ip_and_time_to_console();

#endif // PARSERS_DATA_H