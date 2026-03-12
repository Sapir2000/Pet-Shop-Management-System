#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "log_action.h"
#include "Workers.h"

Worker *loading_worker_file(int *count)
{
	unsigned int level;
	char line[100], user_name[20], name[20], password[20];
	Worker *workers = NULL;
	*count = 0;

	FILE *file = fopen("Workers.txt", "r");
	if (!(file))
	{
		workers = add_worker_to_array(workers, count, "admin", "manager", "12345678", 1);
		log_action("The admin added");
		return workers;
	}

	while (fgets(line, sizeof(line), file))
	{
		if (line[0] == '\n')
			continue;
		if (strncmp(line, "User Name:", 10) == 0)
		{
			sscanf(line, "User Name:  %19[^\n]", user_name);
			fgets(line, sizeof(line), file);
			sscanf(line, "Name:  %19[^\n]", name);
			fgets(line, sizeof(line), file);
			sscanf(line, "Password:  %19[^\n]", password);
			fgets(line, sizeof(line), file);
			sscanf(line, "Level: %u", &level);
			workers = add_worker_to_array(workers, count, user_name, name, password, level);
		}
	}
	fclose(file);
	return workers;
}

Worker *add_worker_to_array(Worker *worker, int *size, char user_name[], char name[], char password[], unsigned int level)
{
	Worker *temp = (Worker *)realloc(worker, ((*size) + 1) * sizeof(Worker));
	if (!temp)
	{
		printf("Realloc failed!\n");
		free(worker);
		return NULL;
	}
	worker = temp;

	strcpy(worker[*size].user_name, user_name);
	strcpy(worker[*size].name, name);
	strcpy(worker[*size].password, password);
	worker[*size].level = level;
	(*size)++;
	return worker;
}

void save_worker_file(Worker *workers, int size)
{
	int i;
	FILE *file = fopen("Workers.txt", "w");
	if (!file)
	{
		printf("Error, workers file failed to open\n");
		return;
	}
	for (i = 0; i < size; i++)
	{
		fprintf(file, "User Name: %s\nName: %s\nPassword: %s\nLevel: %u\n\n",
				workers[i].user_name,
				workers[i].name,
				workers[i].password,
				workers[i].level);
	}
	fclose(file);
}

int enter_worker(Worker *workers, int size)
{
	char user_name[20], password[20], log[100];
	int i, try = 1;

	while (try <= 3)
	{
		printf("\nEnter your user name: ");
		scanf(" %19[^\n]", user_name);
		printf("Enter your password: ");
		scanf(" %19[^\n]", password);

		i = search_worker(workers, size, user_name);
		if (i == -1)
		{
			printf("\xE2\x9D\x8C Wrong user name, you tryied %d times already, %s\n", try, ((try < 3) ? "try again" : "you can't try any more."));
			try++;
		}
		else if ((strcmp(workers[i].password, password)) == 0)
		{
			switch (workers[i].level)
			{
			case 1:
				printf("\n\xE2\x9C\x85 Welcome %s! (Level 1: Manager)\n", workers[i].name);
				break;
			case 2:
				printf("\n\xE2\x9C\x85 Welcome %s! (Level 2: Regular Worker)\n", workers[i].name);
				break;
			case 3:
				printf("\n\xE2\x9C\x85 Welcome %s! (Level 3: Worker in learn)\n", workers[i].name);
				break;
			}
			snprintf(log, sizeof(log), "%s logged in", workers[i].name);
			log_action(log);
			return i;
		}
		else
		{
			printf("\xE2\x9D\x8C Wrong password, you tryied %d times already, %s\n", try, ((try < 3) ? "try again" : "you can't try any more."));
			try++;
		}
	}
	return -1;
}

int search_worker(Worker *workers, int size, char user_name[20])
{
	int i;
	for (i = 0; i < size; i++)
		if ((_stricmp(workers[i].user_name, user_name)) == 0)
			return i;
	return -1;
}

Worker *add_worker(Worker *workers, int *size)
{
	int run_user=1,run_level = 1;
	char log[100];
	Worker new_worker = {0};

	printf("Enter user name: ");
	while(run_user)
	{ 
		scanf(" %19[^\n]", new_worker.user_name);
		while (getchar() != '\n');
		if (strlen(new_worker.user_name) > 10)
			printf("\xE2\x9A\xA0 User name is too long (maximum 10 characters), enter a valid username: ");
		else if (search_worker(workers, *size, new_worker.user_name) == 1)
			printf("\xE2\x9D\x8C This username is taken, please enter another username: ");
		else
			run_user = 0;
	}
	printf("Enter name: ");
	scanf(" %19[^\n]", new_worker.name);
	while (getchar() != '\n');
	while(strlen(new_worker.name) > 10)
	{
		printf("\xE2\x9A\xA0 Name is too long (maximum 10 characters), enter a valid name: ");
		scanf(" %19[^\n]", new_worker.name);
		while (getchar() != '\n');
	}

	printf("Enter password: ");
	scanf(" %19[^\n]", new_worker.password);
	while (getchar() != '\n');
	while (strlen(new_worker.password) > 10)
	{
		printf("\xE2\x9A\xA0 Password is too long (maximum 10 characters), enter a valid password: ");
		scanf(" %19[^\n]", new_worker.password);
		while (getchar() != '\n');
	}
	
	printf("Enter level: ");
	while (run_level)
	{
		if (scanf("%u", &new_worker.level) != 1)
		{
			while (getchar() != '\n');
			printf("\xE2\x9A\xA0 Invalid level input, enter betwin 1-3 level: ");
		}
		else if (new_worker.level < 1 || new_worker.level > 3)
			printf("\xE2\x9A\xA0 Invalid level input, enter betwin 1-3 level: ");
		else
			run_level = 0;
	}

	workers = add_worker_to_array(workers, size, new_worker.user_name, new_worker.name, new_worker.password, new_worker.level);
	printf("\xE2\x9C\x85 Worker %s has added successfully\n", new_worker.user_name);
	snprintf(log, sizeof(log), "%s has added", workers[*size - 1].name);
	log_action(log);

	return workers;
}

Worker *delete_worker(Worker *workers, int *size, unsigned int current_worker, int i)
{
	int j;
	char log[100], name[20];
	strcpy(name, workers[i].name);

	if (i == current_worker)
		printf("\xE2\x9D\x8C You can't delete yourself.\n");
	else if (i >= 0)
	{
		for (j = i; j < *size - 1; j++)
			workers[j] = workers[j + 1];
		(*size)--;
		snprintf(log, sizeof(log), "%s has deleted", name);
		log_action(log);
		printf("\xE2\x9C\x85 successfully delete\n");
	}
	else
		printf("\xE2\x9D\x8C The worker has not found\n");
	return workers;
}