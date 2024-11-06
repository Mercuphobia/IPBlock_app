#ifndef BLOCK_IP_H
#define BLOCK_IP_H


void get_list();
void delete_iptable_rules_chain_and_ipset();
void clear_file_to_run(const char *filename);
void run();
#endif // BLOCK_IP_H