#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "Workers.h"
#include "Customer.h"
#include "log_action.h"
#include "Menu_Workers.h"
#include "Items.h"

void enter_to_system(Worker** workers, int* total_workers, Node** head, TreeNode** inventoryRoot)
{
	int num;
	num = enter_worker(*workers, *total_workers);
	if (num == -1)
		return;
	switch ((*workers)[num].level)
	{
	case 1:
		worker_level_1(workers, total_workers, num, head, inventoryRoot);
		break;
	case 2:
	case 3:
		worker_level_2and3(workers, total_workers, num, head, inventoryRoot);
		break;
	}
}

void basic_worker_menu()
{
	printf("\nWhat do you want to do?\n"
		"1. Search \xF0\x9F\x94\x8D\n"
		"2. Add item \xF0\x9F\x93\xA6\n"
		"3. Add customer \xF0\x9F\x91\xA4\n");
}

void worker_level_1(Worker** workers, int* total_workers, int current_worker, Node** head, TreeNode** inventoryRoot)
{
	int choice, run = 1;
	char log[100];
	while (run)
	{
		int low_stock = 0;
		if (generateLowStockReport(*inventoryRoot) == 1)
		{
			printf("\n\xE2\x9A\xA0  LOW STOCK ALERT \xE2\x9A\xA0\n");
			low_stock = 1;
		}
		basic_worker_menu();
		printf("4. Add worker \xF0\x9F\x91\xB7\n");
		if (low_stock == 1)
			printf("5. See all low stock items \xE2\x9A\xA0\n");
		printf("0. Log out \xF0\x9F\x9A\xAA\n");
		printf("\nChoice: ");
		if (scanf("%d", &choice) != 1)
		{
			while (getchar() != '\n');
			choice = -1;
		}
		printf("\n");
		switch (choice)
		{
		case 1:
			menu_search(*workers, total_workers, current_worker, head, inventoryRoot);
			break;
		case 2:
			add_item_to_store(inventoryRoot, (*workers)[current_worker].name);
			break;
		case 3:
			add_customer(head, (*workers)[current_worker].name);
			break;
		case 4:
			*workers = add_worker(*workers, total_workers);
			break;
		case 5:
			if(low_stock == 1)
			{ 
				printf("\n--- \xE2\x9A\xA0 LOW STOCK ALERT \xE2\x9A\xA0 ---\n");
				PrintGenerateLowStockReport(*inventoryRoot);
				printf("------------------------------\n");
				Edit_Item_menu(inventoryRoot, (*workers)->name);
			}
			else
				printf("Invalid choice \xE2\x9A\xA0\n");
			break;
		case 0:
			run = 0;
			snprintf(log, sizeof(log), "%s logged out", (*workers)[current_worker].name);
			log_action(log);
			break;
		default:
			printf("Invalid choice \xE2\x9A\xA0\n");
			break;
		};
	}
}

void worker_level_2and3(Worker** workers, int* total_workers, int current_worker, Node** head, TreeNode** inventoryRoot)
{
	int choice, run = 1;
	char log[100];
	while (run)
	{
		int low_stock = 0;
		if (generateLowStockReport(*inventoryRoot) == 1)
		{
			printf("\n\xE2\x9A\xA0  LOW STOCK ALERT \xE2\x9A\xA0\n");
			low_stock = 1;
		}
		basic_worker_menu();
		if (low_stock == 1)
			printf("4. See all low stock items \xE2\x9A\xA0\n");
		printf("0. Log out \xF0\x9F\x9A\xAA\n");
		printf("\nChoice: ");
		if (scanf("%d", &choice) != 1)
		{
			while (getchar() != '\n');
			choice = -1;
		}
		printf("\n");

		switch (choice)
		{
		case 1:
			menu_search(*workers, total_workers, current_worker, head, inventoryRoot);
			break;
		case 2:
			add_item_to_store(inventoryRoot, (*workers)[current_worker].name);
			break;
		case 3:
			add_customer(head, (*workers)[current_worker].name);
			break;
		case 4:
			if (low_stock == 1)
			{
				printf("\n--- \xE2\x9A\xA0 LOW STOCK ALERT \xE2\x9A\xA0 ---\n");
				PrintGenerateLowStockReport(*inventoryRoot);
				printf("------------------------------\n");
				if ((*workers)[current_worker].level == 2)
					Edit_Item_menu(inventoryRoot, (*workers)->name);
			}
			else
				printf("Invalid choice \xE2\x9A\xA0\n");
			break;
		case 0:
			run = 0;
			snprintf(log, sizeof(log), "%s logged out", (*workers)[current_worker].name);
			log_action(log);
			break;
		default:
			printf("Invalid choice \xE2\x9A\xA0\n");
			break;
		};
	}
}

void menu_search(Worker* workers, int* total_workers, int current_worker, Node** head, TreeNode** inventoryRoot)
{
	int choice, run = 1;
	while (run)
	{
		printf("Search for:\n"
			"1. Item \xF0\x9F\x93\xA6\n"
			"2. Customer \xF0\x9F\x91\xA4\n");
		if (workers[current_worker].level == 1)
			printf("3. Worker \xF0\x9F\x91\xB7\n");
		printf("0. Go back\n");
		printf("\nChoice: ");
		if (scanf("%d", &choice) != 1)
		{
			while (getchar() != '\n');
			choice = -1;
		}
		printf("\n");
		switch (choice)
		{
		case 1:
			menu_search_item_advanced(inventoryRoot, workers[current_worker].level,workers[current_worker].name,NULL);
			break;
		case 2:
			menu_search_customer(head, workers, current_worker);
			break;
		case 3:
			if (workers[current_worker].level == 1)
			{
				menu_search_worker(workers, total_workers, current_worker);
				break;
			}
			else
			{
				printf("\xE2\x9A\xA0 Invalid choice\n");
				break;
			}
		case 0:
			run = 0;
			break;
		default:
			printf("\xE2\x9A\xA0 Invalid choice\n");
			break;
		}
	}
}

void menu_search_worker(Worker* workers, int* total_workers, int current_worker)
{
	unsigned int level;
	int run = 1, i, choice;
	char user_name[20], data[20], log[100];
	printf("Enter worker user name: ");
	scanf(" %19[^\n]", user_name);
	printf("\n");
	i = search_worker(workers, *total_workers, user_name);
	if (i == -1)
	{
		printf("\xE2\x9D\x8C This user name isn't exist\n");
		return;
	}
	while (run)
	{
		printf("\nData:\n"
			"\tUser name: %s\n"
			"\tName: % s\n"
			"\tPassword: % s\n"
			"\tLevel: % u\n",
			workers[i].user_name, workers[i].name, workers[i].password, workers[i].level);

		printf("\nWhat are you willing to do?\n"
			"1. Edit worker name \xF0\x9F\x93\x9D\n"
			"2. Edit worker password \xF0\x9F\x94\x92\n"
			"3. Edit worker level \xF0\x9F\x8F\x86\n"
			"4. Delete worker \xF0\x9F\x97\x91\n"
			"0. Go back\n");
		printf("\nChoice: ");
		if (scanf("%d", &choice) != 1)
		{
			while (getchar() != '\n');
			choice = -1;
		}
		printf("\n");

		switch (choice)
		{
		case 1:
		{
			printf("Enter new name: ");
			scanf(" %19[^\n]", workers[i].name);
			while (getchar() != '\n');
			while (strlen(workers[i].name) > 10)
			{
				printf("\xE2\x9A\xA0 Name is too long (maximum 10 characters), enter a valid name: ");
				scanf(" %19[^\n]", workers[i].name);
				while (getchar() != '\n');
			}
			snprintf(log, sizeof(log), "%s edit %s name", workers[current_worker].name, workers[i].name);
			printf("\xE2\x9C\x85 Employee name changed successfully\n");
		}
		break;
		case 2:
			printf("Enter new password: ");
			scanf(" %19[^\n]", workers[i].password);
			while (getchar() != '\n');
			while (strlen(workers[i].password) > 10)
			{
				printf("\xE2\x9A\xA0 Password is too long (maximum 10 characters), enter a valid password: ");
				scanf(" %19[^\n]", workers[i].password);
				while (getchar() != '\n');
			}
			snprintf(log, sizeof(log), "%s edit %s password", workers[current_worker].name, workers[i].name);
			printf("\xE2\x9C\x85 Employee password changed successfully\n");
		break;
		case 3:
		{
			int run_level = 1;
			printf("Enter new level (1-3): ");
			while (run_level)
			{
				if (scanf("%u", &level) != 1)
				{
					while (getchar() != '\n');
					printf("\xE2\x9A\xA0 Invalid level input, enter level betwin 1-3: ");
				}
				else if (level < 1 || level > 3)
				{
					printf("\xE2\x9A\xA0 Invalid level input, enter level betwin 1-3: ");
				}
				else
				{
					workers[i].level = level;
					snprintf(log, sizeof(log), "%s edit %s level", workers[current_worker].name, workers[i].name);
					printf("\xE2\x9C\x85 Employee level changed successfully\n");
					run_level = 0;
				}
			}
		}
			break;
		case 4:
			workers = delete_worker(workers, total_workers, current_worker, i);
			run = 0;
			break;
		case 0:
			run = 0;
			break;
		default:
			printf("\xE2\x9A\xA0 Invalid choice\n");
			break;
		}
		if (choice >= 1 && choice <= 3)
			log_action(log);
	}
}

void menu_search_customer(Node** head, Worker* workers, int current_worker)
{
	int run = 1, choice;
	char user_name[20], log[100], data[20];
	Node* current_customer;
	Node* prev_customer = NULL;
	printf("Enter customer user name: ");
	scanf(" %19[^\n]", user_name);
	printf("\n");
	current_customer = find_customer(*head, user_name, &prev_customer);
	if (current_customer == NULL)
	{
		printf("\xE2\x9D\x8C This user name isn't exist\n");
		return;
	}
	display_customer(current_customer);
	while (run)
	{
		if (workers[current_worker].level != 3)
		{
			printf("\nWhat are you willing to do?\n"
				"1. Edit customer user_name \xF0\x9F\x93\x9D\n"
				"2. Delete customer \xF0\x9F\x97\x91\n");
		}
		printf("0. Go back\n");
		printf("\nChoice: ");
		if (scanf("%d", &choice) != 1)
		{
			while (getchar() != '\n');
			choice = -1;
		}
		printf("\n");
		switch (choice) {
		case 1:
			if (workers[current_worker].level != 3)
			{
				printf("Enter new user_name: ");
				scanf(" %19[^\n]", data);
				printf("\n");
				snprintf(log, sizeof(log), "%s edit %s customer name", workers[current_worker].name, current_customer->customer_data->user_name);
				strcpy(current_customer->customer_data->user_name, data);
				log_action(log);
				run = 0;
				break;
			}
		case 2:
			if (workers[current_worker].level != 3)
			{
				remove_customer(head, prev_customer, workers[current_worker].name);
				run = 0;
				break;
			}
		case 0:
			run = 0;
			break;
		default:
			printf("\xE2\x9A\xA0 Invalid choice\n");
			break;
		}
	}
}

void Edit_Item_menu(TreeNode** inventoryRoot, char name[20])
{
	int edit_sn;
	int run = 1;
	while (run)
	{
		printf("\nDo you want to edit an item from these results?\n"
			"Enter its Serial Number (or 0 to cancel): ");

		if (scanf("%ld", &edit_sn) == 1 && edit_sn != 0)
		{
			printf("\n");
			TreeNode* item_to_edit = searchItemBySerialNumber(*inventoryRoot, edit_sn);
			if (item_to_edit != NULL)
			{
				int choice, tempBestSeller;
				char log[100];

				while (run)
				{
					printf("\n1. Edit item name\n"
						"2. Edit price\n"
						"3. Edit stock count\n"
						"4. Edit on sale\n"
						"5. Delete item\n"
						"0. Go back\n"
						"\nChoice: ");
					scanf("%d", &choice);
					printf("\n");
					switch (choice)
					{
					case 1:
					{
						char last_item_name[100];
						strcpy(last_item_name, item_to_edit->dataItem.name);
						printf("\nEnter new name: ");
						scanf(" %99[^\n]", item_to_edit->dataItem.name);
						while (strlen(item_to_edit->dataItem.name) > 10)
						{
							printf("\xE2\x9A\xA0 Name is too long (maximum 10 characters), enter a valid name: ");
							scanf(" %99[^\n]", item_to_edit->dataItem.name);
						}
						snprintf(log, sizeof(log), "%s has edit the name of %s to %s", name, last_item_name, item_to_edit->dataItem.name);
						log_action(log);
					}
					break;
					case 2:
						printf("\nEnter new price: ");
						while (scanf("%lf", &item_to_edit->dataItem.price) != 1 || item_to_edit->dataItem.price<0 || item_to_edit->dataItem.price>999999)
						{
							while (getchar() != '\n');
							printf("\xE2\x9D\x8C Invalid input. Enter a valid price (number between 0 - 999999): ");
						}
						printf("\n\xE2\x9C\x85 Price updated!\n");
						snprintf(log, sizeof(log), "%s has edit the price of %s", name, item_to_edit->dataItem.name);
						log_action(log);
						break;
					case 3:
					{
						printf("\nEnter new stock count: ");
						while (scanf("%d", &item_to_edit->dataItem.stockCount) != 1 || item_to_edit->dataItem.stockCount < 0 || item_to_edit->dataItem.stockCount>9999)
						{
							while (getchar() != '\n');
							printf("\xE2\x9D\x8C Invalid input. Enter stock count (number between 0 - 9999): ");
						}
						if (item_to_edit->dataItem.stockCount > 0)
							item_to_edit->dataItem.isInTheShop = 1;
						else
							item_to_edit->dataItem.isInTheShop = 0;
						printf("\n\xE2\x9C\x85 Stock updated!\n");
						snprintf(log, sizeof(log), "%s has edit the stock of %s", name, item_to_edit->dataItem.name);
						log_action(log);
					}
					break;
					case 4:
						printf("\nEnter if item on sale? (1 = Yes or 0 = No): ");
						while (scanf("%d", &tempBestSeller) != 1 || tempBestSeller < 0 || tempBestSeller>1)
						{
							while (getchar() != '\n');
							printf("\n\xE2\x9A\xA0 Invalid input, Enter if item on sale? (1 = Yes or 0 = No): ");
						}
						item_to_edit->dataItem.onSale = (bool)tempBestSeller;
						printf("\n\xE2\x9C\x85 On sale updated!\n");
						snprintf(log, sizeof(log), "%s has edit on sale for %s", name, item_to_edit->dataItem.name);
						log_action(log);
						break;
					case 5:
					{
						char last_item_name[100];
						strcpy(last_item_name, item_to_edit->dataItem.name);
						*inventoryRoot = delete_item_from_store(*inventoryRoot, item_to_edit->dataItem.serialNumber);
						printf("\xE2\x9C\x85 Item delete!\n\n");
						snprintf(log, sizeof(log), "%s has delete item %s from store", name, last_item_name);
						log_action(log);
						run = 0;
					}
					break;
					case 0:
						run = 0;
						break;
					default:
						printf("\nInvalid choice \xE2\x9A\xA0\n");
						break;
					}
				}
			}
			else
				printf("\n\xE2\x9D\x8C Item not found.\n\n");
		}
		else
		{
			printf("\n");
			run = 0;
		}
	}
}