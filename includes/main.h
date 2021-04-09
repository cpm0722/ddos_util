#ifndef MAIN_H
#define MAIN_H
void get_input(void);
void make_tokens(void);
bool check_options(char *argv[], int argc);
attack_type argv_to_tokens(char *argv[], int argc);
int choose_running_type(void);
attack_type type_choose_attack(void);
void print_main(void);
void print_usage(char *argv[]);
int main(int argc, char *argv[]);
#endif //ifndef MAIN_H
