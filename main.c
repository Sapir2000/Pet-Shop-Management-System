#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <windows.h>
#include "Workers.h"
#include "Customer.h"
#include "Menu_Customer.h"
#include "Menu_Workers.h"
#include "Items.h"

int main()
{
	SetConsoleOutputCP(65001);
	int total_workers, run = 1, choice;
	Worker* workers = loading_worker_file(&total_workers);
	TreeNode* inventoryRoot = loadItemsTree();
	Node* first_customer = loading_customer_file(inventoryRoot);

	printf("\xF0\x9F\x90\xB1 Welcome to the Pet Shop! \xF0\x9F\x90\xB6\n");
	while (run)
	{
		printf("\nChoose your user:\n");
		printf("1. Customer \xF0\x9F\x91\xA4\n");
		printf("2. Worker \xF0\x9F\x91\xB7\n");
		printf("\nChoice: ");

		if (scanf("%d", &choice) != 1) {
			while (getchar() != '\n');
			choice = -1;
		}

		switch (choice)
		{
		case 1:
			customer_enter_to_system(&first_customer, inventoryRoot);
			run = 0;
			break;
		case 2:
			enter_to_system(&workers, &total_workers, &first_customer, &inventoryRoot);
			run = 0;
			break;
		default:
			printf("\xE2\x9A\xA0 Invalid choice\n");
			break;
		}
	}
	save_worker_file(workers, total_workers);
	save_customer_file(first_customer);
	FILE* itemsFile = fopen("Items.bin", "wb");
	if (itemsFile != NULL)
	{
		SaveItemsTree(inventoryRoot, itemsFile);
		fclose(itemsFile);
	}
	else
		printf("Error, could not save inventory to binary file\n");

	free_list(first_customer);
	freeTree(inventoryRoot);
	free(workers);

	return 0;
}