#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include "log.h"
#include "parsers_data.h"



// run board//
// #define IPSET_LIST_NO_STDOUT "/userfs/bin/ipset list %s > /dev/null 2>&1"
// #define IPSET_CREATE "/userfs/bin/ipset create %s hash:ip"
// #define IPSET_ADD "/userfs/bin/ipset add %s %s"
// #define IPSET_DELETE_RULE "/userfs/bin/ipset destroy %s_%ld"
// #define IPSET_TEST_RULE "/userfs/bin/ipset test %s %s > /dev/null 2>&1"

//run vmware//
#define IPSET_LIST_NO_STDOUT "ipset list %s > /dev/null 2>&1"
#define IPSET_CREATE "ipset create %s hash:ip"
#define IPSET_ADD "ipset add %s %s"
#define IPSET_DELETE_RULE "ipset destroy %s_%ld"
#define IPSET_TEST_RULE "ipset test %s %s > /dev/null 2>&1"


#define IP_TABLES_ADD_INPUT "iptables -I INPUT -m set --match-set %s_%ld src -j DROP"
#define IP_TABLES_ADD_OUTPUT "iptables -I OUTPUT -m set --match-set %s_%ld src -j DROP"
#define IP_TABLES_ADD_FORWARD "iptables -I FORWARD -m set --match-set %s_%ld src -j DROP"

#define IP_TABLES_DELETE_INPUT "iptables -D INPUT -m set --match-set %s_%ld src -j DROP 2>/dev/null"
#define IP_TABLES_DELETE_OUTPUT "iptables -D OUTPUT -m set --match-set %s_%ld src -j DROP 2>/dev/null"
#define IP_TABLES_DELETE_FORWARD "iptables -D FORWARD -m set --match-set %s_%ld src -j DROP 2>/dev/null"


// #define IP_TXT_PATH "./data/ip.txt"
// #define CHECK_TXT_PATH "./data/check.txt"

#define IP_TXT_PATH "../../block_app/data/ip.txt"
#define CHECK_TXT_PATH "../../block_app/data/check.txt"


#define REST_TIME_BETWEEN_RUN 30
int num_struct = 0;
char command[256];

int get_day_number(const char* day) {
    if (strcmp(day, "Monday") == 0) return 0;
    if (strcmp(day, "Tuesday") == 0) return 1;
    if (strcmp(day, "Wednesday") == 0) return 2;
    if (strcmp(day, "Thursday") == 0) return 3;
    if (strcmp(day, "Friday") == 0) return 4;
    if (strcmp(day, "Saturday") == 0) return 5;
    if (strcmp(day, "Sunday") == 0) return 6;
    return -1;
}

long convert_to_seconds(const char* day, const char* time) {
    int day_number = get_day_number(day);
    if (day_number == -1) {
        printf("Invalid day: %s\n", day);
        return -1;
    }
    int hours, minutes;
    sscanf(time, "%d:%d", &hours, &minutes);
    long total_seconds = day_number * 86400 + hours * 3600 + minutes * 60;
    return total_seconds;
}

long get_current_time_in_seconds() {
    time_t now = time(NULL);
    struct tm *tm_now = localtime(&now);
    int day_number = tm_now->tm_wday - 1;
    if (day_number < 0) day_number = 6;
    long total_seconds = day_number * 86400 + tm_now->tm_hour * 3600 + tm_now->tm_min * 60 + tm_now->tm_sec;
    return total_seconds;
}

int ipset_exists(const char* ipset_name) {
    snprintf(command, sizeof(command), IPSET_LIST_NO_STDOUT, ipset_name);
    int result = system(command);
    return result == 0;
}

void create_ipset(const char* ipset_name) {
    if (!ipset_exists(ipset_name)) {
        snprintf(command, sizeof(command), IPSET_CREATE, ipset_name);
        system(command);
    }
}

void add_ip_to_ipset(const char* ipset_name, const char* ip) {
    snprintf(command, sizeof(command), IPSET_TEST_RULE, ipset_name, ip);
    if (system(command) != 0) { 
        snprintf(command, sizeof(command), IPSET_ADD, ipset_name, ip);
        system(command);
    }
}

int rule_iptables_exists(const char* url,long start_time) {
    snprintf(command, sizeof(command), "iptables -L | grep -q %s_%ld", url, start_time);
    return system(command) == 0;
}


void clear_file_to_run(const char *filename) {
    FILE *check_file = fopen(filename, "w");
    if (check_file == NULL) {
        perror("Unable to open file");
        return;
    }
    fclose(check_file);
}

bool is_line_in_file(FILE *file, const char *line) {
    char buffer[256];
    rewind(file);
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        if (strcmp(buffer, line) == 0) {
            return true;
        }
    }
    return false;
}

void get_list() {
    web_block_info *list = read_web_block_info(IP_TXT_PATH, &num_struct);
    FILE *check_file = fopen(CHECK_TXT_PATH, "a+");
    if (check_file == NULL) {
        printf("Unable to open file\n");
        return;
    }
    for (int i = 0; i < num_struct; i++) {
        char ipset_name[256];
        snprintf(ipset_name, sizeof(ipset_name), "%s_%ld", list[i].url, convert_to_seconds(list[i].start_day,list[i].start_time));
        create_ipset(ipset_name);
        add_ip_to_ipset(ipset_name, list[i].ip);
        char line[256];
        snprintf(line, sizeof(line), "%s, %ld, %ld\n",
                 list[i].url,
                 convert_to_seconds(list[i].start_day, list[i].start_time),
                 convert_to_seconds(list[i].end_day, list[i].end_time));
        if (!is_line_in_file(check_file, line)) {
            fprintf(check_file, "%s", line);
        }
    }
    fclose(check_file);
}

void run() {
    printf_to_file(IP_TXT_PATH);
    get_list();
    int num_struct = 0;
    check *list = read_check_list(CHECK_TXT_PATH, &num_struct);
    int *rule_active = malloc(num_struct * sizeof(int));
    memset(rule_active, 0, num_struct * sizeof(int));
    time_t current_time = time(NULL);
    long local_time = get_current_time_in_seconds();

    for (int i = 0; i < num_struct; i++) {
        if (local_time >= list[i].start_time_block && local_time <= list[i].end_time_block) {
            LOG(LOG_LVL_ERROR, "test_add_rules_1: %s, %s, %d\n", __FILE__, __func__, __LINE__);
            if (!rule_iptables_exists(list[i].url, list[i].start_time_block)) {
                    LOG(LOG_LVL_ERROR, "test_add_rules_3: %s, %s, %d\n", __FILE__, __func__, __LINE__);
                    snprintf(command, sizeof(command), IP_TABLES_ADD_INPUT, list[i].url, list[i].start_time_block);
                    system(command);
                    snprintf(command, sizeof(command), IP_TABLES_ADD_OUTPUT, list[i].url, list[i].start_time_block);
                    system(command);
                    snprintf(command, sizeof(command), IP_TABLES_ADD_FORWARD, list[i].url, list[i].start_time_block);
                    system(command);
                    printf("Added rule to block IP in ipset %s_%ld\n", list[i].url, list[i].start_time_block);
            }
        } else {
            if (rule_iptables_exists(list[i].url, list[i].start_time_block)){
                    LOG(LOG_LVL_ERROR, "test_delete_rules_3: %s, %s, %d\n", __FILE__, __func__, __LINE__);
                    snprintf(command, sizeof(command), IP_TABLES_DELETE_INPUT, list[i].url, list[i].start_time_block);
                    system(command);
                    snprintf(command, sizeof(command), IP_TABLES_DELETE_OUTPUT, list[i].url, list[i].start_time_block);
                    system(command);
                    snprintf(command, sizeof(command), IP_TABLES_DELETE_FORWARD, list[i].url, list[i].start_time_block);
                    system(command);
                    printf("Removed rule to unblock IP in ipset %s_%ld\n", list[i].url, list[i].start_time_block);
            }
        }
    }
    free(rule_active);
}

void delete_iptable_rules_chain_and_ipset(){
    LOG(LOG_LVL_ERROR, "test_delete_iptable_rules_chain_and_ipset: %s, %s, %d\n", __FILE__, __func__, __LINE__);
    check *list = read_check_list(CHECK_TXT_PATH, &num_struct);
    for(int i=0;i<num_struct;i++){
        char name_ipset[256];
        snprintf(name_ipset, sizeof(name_ipset), "%s_%ld", list[i].url, list[i].start_time_block);
        if(ipset_exists(name_ipset)){
            snprintf(command, sizeof(command), IP_TABLES_DELETE_INPUT, list[i].url, list[i].start_time_block);
            system(command);
            snprintf(command, sizeof(command), IP_TABLES_DELETE_OUTPUT, list[i].url, list[i].start_time_block);
            system(command);
            snprintf(command, sizeof(command), IP_TABLES_DELETE_FORWARD, list[i].url, list[i].start_time_block);
            system(command);
            snprintf(command, sizeof(command), IPSET_DELETE_RULE, list[i].url, list[i].start_time_block);
            system(command);
        }
        else{
            printf("ipset %s_%ld does not exist.\n", list[i].url, list[i].start_time_block);
        }
    }
}
