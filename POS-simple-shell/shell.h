/*
 * 2. projekt do predmetu POS (Pokročilé operačné systémy)
 * Jednoduchý shell
 * Michal Gabonay (xgabon00@stud.fit.vutbr.cz)
 * Vysoké Učení Technické v Brne
 * Fakulta Informačných technológií
 * date: 12.4.2018
 */

#define _POSIX_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <sys/stat.h> 
#include <fcntl.h>

#define INPUT_MAX 513
#define INPUT_DELIM " \t\r\n\a"
#define BUFSIZE 10

// type for shared data
typedef struct {
	pthread_mutex_t mutex;
    pthread_cond_t condition;
	bool stopped;
    char input[INPUT_MAX];
    pid_t pid;
} shared_data_t;

// type for program
typedef struct {
	int argc;
	char **argv;
	char *redirect_in;
	char *redirect_out;
	bool on_background;
} program_t;

static void child_handler();
static void int_handler();
void *thr_read(void *thr_data);
void *thr_perform(void *thr_data);
void run_signal(pthread_mutex_t *mutex, pthread_cond_t *condition);
void run_wait(pthread_mutex_t *mutex, pthread_cond_t *condition);
void parse_input(shared_data_t *data, program_t *prog);
void shell_launch(program_t *prog, shared_data_t *data);