/*
 * 2. projekt do predmetu POS (Pokročilé operačné systémy)
 * Jednoduchý shell
 * Michal Gabonay (xgabon00@stud.fit.vutbr.cz)
 * Vysoké Učení Technické v Brne
 * Fakulta Informačných technológií
 * date: 12.4.2018
 */

#include "shell.h"

//global variable - shared data between threads
shared_data_t shared_data;

static void  int_handler()
{
	// kill running procces in front, if nothing running, just go to new line
	if(shared_data.pid > 0){
		fprintf(stderr, "\n");
		kill(shared_data.pid, SIGINT);
	}else{
		fprintf(stderr, "\n$ ");
	}
}

static void child_handler()
{
    pid_t pid;
    int status;

    pid = wait(&status);

    // no proccess is waiting (nothing on background)
    if (pid < 0)
    {
    	return;
    }

    if (WIFEXITED(status)) {
		fprintf(stderr, "%d: normal termination (exit code = %d)\n", pid, WEXITSTATUS(status));
		fprintf(stderr, "$ ");
	} else if (WIFSIGNALED(status)) {
		fprintf(stderr, "%d: signal termination (signal = %d)\n", pid, WTERMSIG(status));
		fprintf(stderr, "$ ");
	} else if (WIFSTOPPED(status)) {
		fprintf(stderr, "%d: stopped by signal (signal = %d)\n", pid, WSTOPSIG(status));
		fprintf(stderr, "$ ");
	} else {
		fprintf(stderr, "%d: unknown type of termination\n", pid);
		fprintf(stderr, "$ ");
	} 

	return;  
}

void parse_input(shared_data_t *data, program_t *prog)
{
	int position = 0;
	int bufsize = BUFSIZE;
	char **tokens = malloc(bufsize * sizeof(char*));
	char *token;
	bool next_out = false;
	char *pointo;
	char *pointi;

	if (!tokens) {
		fprintf(stderr, "Allocation error\n");
	    exit(EXIT_FAILURE);
	}

	token = strtok(data->input, INPUT_DELIM);
  	while (token != NULL) {
	    pointo = strchr(token, '>');
	    pointi = strchr(token, '<');
	    	 
	    if (strcmp(token, "&") == 0)
	    {
	    	prog->on_background = true;
	    } else if (next_out)
	    {
	    	prog->redirect_out = token;
	    	next_out = false;
	    } else if (pointo != NULL)
	    {
	    	if (strlen(token) == 1)
	    	{
	    		// output file on next token
	    		next_out = true;
	    	} else if (token[0] == '>')
	    	{
	    		prog->redirect_out = ++pointo;
	    	} else {
	    		prog->redirect_out = ++pointo;
	    		*(pointo-1) = 0;
	    		tokens[position] = token;
	    		position++;
	    	}
	    } else if (pointi != NULL)
	    {
	    	if (strlen(token) == 1)
	    	{
	    		// input file was previous token
	    		prog->redirect_in = tokens[--position];
	    	} else {
	    		prog->redirect_in = token;
	    		prog->redirect_in[strlen(token)-1] = 0;
	    	}
	    } else {
 			tokens[position] = token;
	    	position++;
	    }

	    if (position >= bufsize) {
	      	bufsize += BUFSIZE;
	      	tokens = realloc(tokens, bufsize * sizeof(char*));
	      	if (!tokens) {
		        fprintf(stderr, "Allocation error\n");
		        exit(EXIT_FAILURE);
	      	}
	    }

    	token = strtok(NULL, INPUT_DELIM);
  	}
  	tokens[position] = NULL;
  	prog->argv = tokens;
  	prog->argc = position;
}

void shell_launch(program_t *prog, shared_data_t *data)
{
	pid_t pid;
  	int in_file, out_file;

  	pid = fork();
  	if (pid == 0) {
  		// open input and output files
		if(prog->redirect_out != NULL)
		{
			out_file = open(prog->redirect_out, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
			if(out_file == -1)
			{
				perror("error open output file");
				exit(1);
			}

			if(dup2(out_file, 1) == -1)
			{
				perror("error dup output");
				exit(1);
			}
		}
		if(prog->redirect_in != NULL)
		{
			in_file = open(prog->redirect_in, O_RDONLY);
			if(in_file == -1)
			{
				perror("error open input file");
				exit(1);
			}

			if(dup2(in_file, 0) == -1)
			{
				perror("error dup input");
				exit(1);
			}
		}

    	// Child process
    	// printf("--executing: %d\n", (int) (int)getpid());
    	if (execvp(prog->argv[0], prog->argv) == -1) {
      		perror("lsh");
    	}

    	if(prog->redirect_in != NULL)
			close(in_file);

		if(prog->redirect_out != NULL)
			close(out_file);

    	exit(EXIT_FAILURE);
  	} else if (pid < 0) {
    	// Error forking
    	perror("lsh");
    	data->stopped = true;
  	} else {
    	// Parent process
    	if(!prog->on_background)
		{
			pthread_mutex_lock(&(data->mutex));
			data->pid = pid;
			pthread_mutex_unlock(&(data->mutex));
			// printf("--%s: %d\n", "waiting ...",data->pid);
			waitpid(data->pid, NULL, 0);
			// printf("--%s\n", "stop waiting ...");
			pthread_mutex_lock(&(data->mutex));
			data->pid = 0;
			pthread_mutex_unlock(&(data->mutex));
		} else {
		    printf("[process %d started on background]\n", pid);
		    data->pid = 0;                           //creates a new process group leader pid
		}
  	}
 }

/* this function is run by the read thread */
void *thr_read(void *thr_data)
{
	shared_data_t *data = (shared_data_t *)thr_data;
	int nread;

	while (!data->stopped)
	{
		memset(data->input, 0, INPUT_MAX);
		printf("$ ");
		fflush(stdout);

		nread=read(0,data->input,INPUT_MAX);
		if(nread == -1)
		{
			fprintf(stderr, "An read error occured\n");
			while( getchar() != '\n' );
			continue;
		}

		// emplty input - do nothing, go to new line
		if (data->input[0] == EOF || data->input[0] == '\n') 
		{
			continue;
		}

		if(nread == INPUT_MAX)
		{
			fprintf(stderr, "Too long input\n");
			while(getchar() != '\n');
			continue;
		}

		// signal to perform input
		run_signal(&(data->mutex), &(data->condition));	

		// wait until input is performed
		run_wait(&(data->mutex), &(data->condition));
	}
	return NULL;
}

/* this function is run by the perform thread */
void *thr_perform(void *thr_data)
{
	shared_data_t *data = (shared_data_t *)thr_data;
	program_t *p = malloc(sizeof(program_t));

	if (!p) {
		fprintf(stderr, "Allocation error\n");
		exit(EXIT_FAILURE);
	}

	while(!data->stopped)
	{
		p->on_background = false;
		p->redirect_in = NULL;
		p->redirect_out = NULL;
		data->pid = 0;
		shared_data.pid = 0;

		run_wait(&(data->mutex), &(data->condition));
		if(data->stopped)
			break;

		parse_input(data, p);

		if (p->argv[0] == NULL) {
		    // An empty command was entered.
			run_signal(&(data->mutex), &(data->condition));
			continue;
		} else if (strcmp(p->argv[0], "exit") == 0)
		{
			data->stopped = true;
			run_signal(&(data->mutex), &(data->condition)); // end but if proc on bacground, run it
			break;
		}

		// printf("%s\n", "--start launcher");
		shell_launch(p, data);
		// printf("%s\n", "--stop launcher");

		// printf("bcg: %s\n", p->on_background ? "true" : "false");
		// printf("redirect in: %s\n", p->redirect_in);
		// printf("redirect out: %s\n", p->redirect_out);
		// printf("argc: %d\n", p->argc);
		// for (int i = 0; i < p->argc; ++i)
		// {
		// 	printf("argv[%d]: %s\n", i, p->argv[i]);
		// }

		run_signal(&(data->mutex), &(data->condition));
	}

	return NULL;
}

void run_signal(pthread_mutex_t *mutex, pthread_cond_t *condition)
{
	pthread_mutex_lock(mutex);
	pthread_cond_signal(condition);
	pthread_mutex_unlock(mutex);
}

void run_wait(pthread_mutex_t *mutex, pthread_cond_t *condition)
{
	pthread_mutex_lock(mutex);
	pthread_cond_wait(condition, mutex);
	pthread_mutex_unlock(mutex);
}

int main(void)
{
	pthread_t thread_read, thread_perform;

	shared_data.stopped = false;

	/* Establish handler for children on background. */
	struct sigaction sa_child;
	sigemptyset(&sa_child.sa_mask);
	sa_child.sa_flags = 0;
	sa_child.sa_handler = child_handler;
	sigaction(SIGCHLD, &sa_child, NULL);

	/* Establish handler for ctrl-c exit. */
	struct sigaction sa_int;
	sigemptyset(&sa_int.sa_mask);
	sa_int.sa_flags = 0;
	sa_int.sa_handler = int_handler;
	sigaction(SIGINT, &sa_int, NULL);

	// initalize mutex for shared data
	if (pthread_mutex_init(&(shared_data.mutex), NULL) != 0)
    {
        fprintf(stderr, "Error init mutex\n");
        return 1;
    }

    // initalize condition for shared data
    if (pthread_cond_init(&(shared_data.condition), NULL) != 0)
    {
        fprintf(stderr, "Error init condition\n");
        return 1;
    }

	/* create a read thread which executes thr_read(&shared_data) */
	if(pthread_create(&thread_read, NULL, thr_read, &shared_data)) 
	{
		fprintf(stderr, "Error creating thread\n");
		return 1;
	}
	/* create a perform thread which executes thr_perform(&shared_data) */
	if(pthread_create(&thread_perform, NULL, thr_perform, &shared_data)) 
	{
		fprintf(stderr, "Error creating thread\n");
		return 1;
	}
	/* wait for the read thread to finish */
	if(pthread_join(thread_read, NULL)) 
	{
		fprintf(stderr, "Error joining thread\n");
		return 1;
	}
	/* wait for the perform thread to finish */
	if(pthread_join(thread_perform, NULL)) 
	{
		fprintf(stderr, "Error joining thread\n");
		return 1;
	}

	pthread_mutex_destroy(&(shared_data.mutex));
	pthread_cond_destroy(&(shared_data.condition));

	return 0;
}