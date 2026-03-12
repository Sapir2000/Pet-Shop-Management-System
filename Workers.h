#pragma once

typedef struct worker
{
	char user_name[20];
	char name[20];
	char password[20];
	unsigned int level;
} Worker;

Worker *loading_worker_file(int *count);
Worker *add_worker_to_array(Worker *worker, int *size, char user_name[], char name[], char password[], unsigned int level);
void save_worker_file(Worker *workers, int size);
int enter_worker(Worker *workers, int size);
int search_worker(Worker *workers, int size, char user_name[20]);
Worker *add_worker(Worker *workers, int *size);
Worker *delete_worker(Worker *workers, int *size, unsigned int current_worker, int i);