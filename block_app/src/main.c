#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "parsers_option.h"
#include "file_process.h"
#include "get_data.h"
#include "parsers_data.h"
#include "log.h"
#include "block_ip.h"
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <netinet/in.h>
#include <linux/netfilter.h>
#include <libnetfilter_queue/libnetfilter_queue.h>
#include <arpa/inet.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <string.h>
#include "dns.h"
#include "packet_process.h"


#define SRC_WEB_BLOCK_PATH "../../webserver/config/url_data.txt"
#define DES_WEB_BLOCK_PATH "../../block_app/data/block_web.txt"
#define BLOCK_WEB "../../block_app/data/block_web.txt"
#define IP_FILE "../../block_app/data/ip.txt"
#define CHECK_FILE "../../block_app/data/check.txt"
#define FILE_DATA "../../block_app/data/data.txt"


//block_app

// int main(int argc, char *argv[]) {
//     if(argc > 1){
//         parsers_option(argc, argv);
//     }
//     else {
//         signal(SIGINT, delete_iptable_rules_chain_and_ipset);
//         while(1){
//             clear_file_to_run(BLOCK_WEB);
//             transfer_data(SRC_WEB_BLOCK_PATH,DES_WEB_BLOCK_PATH);
//             clear_file_to_run(IP_FILE);
//             clear_file_to_run(CHECK_FILE);
//             run();
//             sleep(1);
//         }
//     }
// }

void app2(){
    int c = 0;
    while(1){
        printf("%d\n",c);
        c+=1;
        sleep(1);
    }
}

// // Hàm cho app 1
int main(int argc, char *argv[]) {
    //app1
    LOG(LOG_LVL_ERROR, "testapp1_2: %s, %s, %d\n", __FILE__, __func__, __LINE__);
    signal(SIGINT, cleanup);
    clear_file_to_run(FILE_DATA);
    clear_file_to_run(BLOCK_WEB);
    transfer_data(SRC_WEB_BLOCK_PATH,DES_WEB_BLOCK_PATH);
    start_packet_capture();
    free_recorded_list();
}


// pthread_t thread1, thread2;
// volatile sig_atomic_t sigint_received = 0;

// void* app1(void* arg) {
//     //signal(SIGINT,cleanup);
//     clear_file_to_run(DATA_FILE);
//     LOG(LOG_LVL_ERROR, "testapp1_1: %s, %s, %d\n", __FILE__, __func__, __LINE__);
//     start_packet_capture();
//     LOG(LOG_LVL_ERROR, "testapp1_2: %s, %s, %d\n", __FILE__, __func__, __LINE__);
// }

// void* app2(void* arg) {
//     //signal(SIGINT,delete_iptable_rules_chain_and_ipset);
//     while (1) {
//         clear_file_to_run(BLOCK_WEB);
//         transfer_data(SRC_WEB_BLOCK_PATH, DES_WEB_BLOCK_PATH);
//         clear_file_to_run(IP_FILE);
//         clear_file_to_run(CHECK_FILE);
//         run();
//         sleep(4);
//     }
// }

// void sigint_handler(int sig) {
//     sigint_received = 1;
//     cleanup();
//     sleep(2);
//     delete_iptable_rules_chain_and_ipset();
//     exit(0);
// }


// int main(int argc, char *argv[]) {
//     parsers_option(argc, argv);
//     LOG(LOG_LVL_ERROR, "testmain1: %s, %s, %d\n", __FILE__, __func__, __LINE__);
//     signal(SIGINT, sigint_handler);

//     pthread_create(&thread1, NULL, app1, NULL);
//     pthread_create(&thread2, NULL, app2, NULL);

//     pthread_join(thread1, NULL);
//     pthread_join(thread2, NULL);

//     return 0;
// }

