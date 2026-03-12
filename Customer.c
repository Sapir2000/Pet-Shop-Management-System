#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "log_action.h"
#include "Customer.h"
#include "Menu_Customer.h"

Node* loading_customer_file(TreeNode* inventoryRoot)
{
	unsigned int i;
	char line[512], * ptr;
	Node* head = NULL, * tail = NULL, * node;
	Customer* customer = NULL;
	ItemNode* last_item = NULL;
	FILE* file = fopen("Customer.txt", "r");
	if (!(file))
	{
		file = fopen("Customer.txt", "w");
		if (file) fclose(file);
		return NULL;
	}
	while (fgets(line, sizeof(line), file))
	{
		if (line[0] == '\n') continue;
		if (strncmp(line, "User Name:", 10) == 0)
		{
			customer = (Customer*)malloc(sizeof(Customer));
			if (!customer)
				return head;
			customer->items_head = NULL;
			customer->items_count = 0;
			sscanf(line, "User Name: %19[^\n]", customer->user_name);
			fgets(line, sizeof(line), file);
			sscanf(line, "Join Date: %s", customer->join_date);
			fgets(line, sizeof(line), file);
			sscanf(line, "Count: %u", &customer->items_count);
			fgets(line, sizeof(line), file);
			ptr = line + 6;
			last_item = NULL;
			for (i = 0; i < customer->items_count; i++)
			{
				int chars = 0;
				long sn;
				char saved_name[50] = "Unknown";
				ItemNode* new_item = (ItemNode*)malloc(sizeof(ItemNode));
				if (!new_item)
					break;
				new_item->next = NULL;
				memset(new_item->data.bought_date, 0, sizeof(new_item->data.bought_date));
				while (*ptr == ' ' || *ptr == ',') ptr++;
				if (sscanf(ptr, "%ld-%[^[][%[^]]]%n", &sn, saved_name, new_item->data.bought_date, &chars) >= 3)
				{
					ptr += chars;
					TreeNode* found_item = searchItemBySerialNumber(inventoryRoot, sn);
					if (found_item != NULL)
						new_item->data.item_addr = createItemNode(found_item->dataItem);
					else
					{
						Item dummyItem = { 0 };
						dummyItem.serialNumber = sn;
						strcpy(dummyItem.name, saved_name);
						strcpy(dummyItem.category, "N/A");
						new_item->data.item_addr = createItemNode(dummyItem);
					}
					if (customer->items_head == NULL)
						customer->items_head = new_item;
					else
						last_item->next = new_item;
					last_item = new_item;
				}
				else
					free(new_item);
			}

			fgets(line, sizeof(line), file);
			sscanf(line, "Money Spent: %f", &customer->money_spent);
			node = createNode(customer);
			if (head == NULL) head = node;
			else tail->next = node;
			tail = node;
		}
	}
	fclose(file);
	return head;
}

Node* createNode(Customer* customer)
{
	Node* newNode = (Node*)malloc(sizeof(Node));
	if (newNode == NULL)
	{
		printf("Node creation did not succeed\n");
		return NULL;
	}
	newNode->customer_data = customer;
	newNode->next = NULL;
	return newNode;
}

void display_customer(Node* customer)
{
	printf("Customer: %s (Joined: %s)\n", customer->customer_data->user_name, customer->customer_data->join_date);
	printf("\t  Spent: %.2f\n", customer->customer_data->money_spent);
	printf("\t  Items (%d):\n", customer->customer_data->items_count);

	ItemNode* current_item = customer->customer_data->items_head;
	while (current_item != NULL)
	{

		if (current_item->data.item_addr != NULL) {
			printf("\t  - %s (Date: %s)\n", current_item->data.item_addr->dataItem.name, current_item->data.bought_date);
		}
		current_item = current_item->next;
	}
	printf("\n");
}

void display_customer_items(Node* customer)
{
	printf("\nPurchase history:\n");
	ItemNode* current_item = customer->customer_data->items_head;
	while (current_item != NULL)
	{
		if (current_item->data.item_addr != NULL)
			printf("%ld - %s (Bought: %s)\n",current_item->data.item_addr->dataItem.serialNumber ,current_item->data.item_addr->dataItem.name, current_item->data.bought_date);
		current_item = current_item->next;
	}
}

void free_list(Node* head)
{
	Node* curr = head;
	while (curr)
	{
		Node* temp = curr;
		curr = curr->next;

		if (temp->customer_data->items_head)
		{
			ItemNode* item_curr = temp->customer_data->items_head;
			while (item_curr)
			{
				ItemNode* item_temp = item_curr;
				item_curr = item_curr->next;
				free(item_temp);
			}
		}
		free(temp->customer_data);
		free(temp);
	}
}

void save_customer_file(Node* head)
{
	Node* curr = head;
	FILE* file = fopen("Customer.txt", "w");
	if (!file)
	{
		printf("Error, workers file failed to open\n");
		return;
	}
	ItemNode* item_curr;
	while (curr != NULL)
	{
		fprintf(file, "User Name: %s\nJoin Date: %s\nCount: %u\nItem: ",
			curr->customer_data->user_name,
			curr->customer_data->join_date,
			curr->customer_data->items_count);
		item_curr = curr->customer_data->items_head;
		while (item_curr != NULL)
		{
			if (item_curr->data.item_addr != NULL) {
				fprintf(file, "%ld-%s[%s],",
				item_curr->data.item_addr->dataItem.serialNumber,
				item_curr->data.item_addr->dataItem.name,
				item_curr->data.bought_date);
			}
			item_curr = item_curr->next;
		}
		fprintf(file, "\nMoney Spent: %.2f\n\n", curr->customer_data->money_spent);
		curr = curr->next;
	}
	fclose(file);
}

Node* find_customer(Node* head, char user_name[], Node** prev)
{
	Node* current_customer = head;
	*prev = NULL;

	while (current_customer != NULL)
	{
		if (strcmp(current_customer->customer_data->user_name, user_name) == 0)
			return current_customer;
		*prev = current_customer;
		current_customer = current_customer->next;
	}
	return NULL;
}

void add_customer(Node** head, char current_worker_name[])
{
	int run_user = 1;
	char log[100], user_name[20];
	Node* customer_exists, * prev;

	printf("Enter customer user_name: ");
	while (run_user)
	{
		scanf(" %19[^\n]", user_name);
		while (getchar() != '\n');
		if (strlen(user_name) > 10)
			printf("\xE2\x9A\xA0 User name is too long (maximum 10 characters), enter a valid username: ");
		else if (find_customer(*head, user_name, &prev) != NULL)
			printf("\xE2\x9D\x8C This username is taken, please enter another username: ");
		else
			run_user = 0;
	}

	add_customer_to_list(head, user_name);
	printf("\xE2\x9C\x85 Customer %s has added successfully \n", user_name);

	snprintf(log, sizeof(log), "%s added new customer: %s", current_worker_name, user_name);
	log_action(log);
}

void add_customer_to_list(Node** head, char user_name[])
{
	Customer* customer = (Customer*)malloc(sizeof(Customer));
	char* current_time = organisde_current_date();

	strcpy(customer->user_name, user_name);
	strcpy(customer->join_date, current_time);
	customer->items_count = 0;
	customer->items_head = NULL;
	customer->money_spent = 0.0;

	Node* newNode = createNode(customer);

	if (*head == NULL) {
		*head = newNode;
		return;
	}

	Node* temp = *head;
	while (temp->next != NULL) {
		temp = temp->next;
	}
	temp->next = newNode;
}

void remove_customer(Node** head, Node* prev, char current_worker_name[])
{
	Node* customer_to_delete = NULL;
	char log[100];

	if (prev == NULL)
	{
		customer_to_delete = (*head);
		*head = customer_to_delete->next;
	}
	else
	{
		customer_to_delete = prev->next;
		prev->next = customer_to_delete->next;
	}
	printf("\xE2\x9C\x85\n The customer %s has deleted successfully\n", customer_to_delete->customer_data->user_name);
	snprintf(log, sizeof(log), "%s deleted the customer %s", current_worker_name, customer_to_delete->customer_data->user_name);
	log_action(log);
	customer_to_delete->next = NULL;
	free_list(customer_to_delete);
	return;
}

Node* enter_customer(Node* head)
{
	Node* current = NULL, * prev = NULL;
	char user_name[20], log[100];
	int try = 1;

	while (try <= 3)
	{
		printf("\nPlease Enter your user name: ");
		scanf(" %19[^\n]", user_name);
		printf("\n");
		if ((current = find_customer(head, user_name, &prev)) == NULL)
		{
			printf("\xE2\x9D\x8C User does not exist, you tried %d times already, %s\n", try, ((try < 3) ? "try again" : "you can't try any more."));
			try++;
		}
		else
		{
			printf("Welcome %s!! \xF0\x9F\x90\xB9\n", current->customer_data->user_name);
			snprintf(log, sizeof(log), "Customer %s logged in", current->customer_data->user_name);
			log_action(log);
			return current;
		}
	}
	return NULL;
}

ItemNode* find_customer_item(ItemNode* head, TreeNode* item_addr, ItemNode** prev)
{
	ItemNode* current_item = head;
	*prev = NULL;

	while (current_item != NULL)
	{
		if (current_item->data.item_addr == item_addr)
			return current_item;
		*prev = current_item;
		current_item = current_item->next;
	}
	return NULL;
}

void remove_customer_item(Customer* customer, ItemNode* item_to_remove, ItemNode* prev)
{
	char log[100];
	if (prev == NULL)
		customer->items_head = item_to_remove->next;
	else
		prev->next = item_to_remove->next;

	snprintf(log, sizeof(log), "The customer %s refunded the %s", customer->user_name, item_to_remove->data.item_addr->dataItem.name);
	log_action(log);
	customer->items_count--;
	free(item_to_remove);
	return;
}


bool is_within_14_days(char* bought_date_str) {
	int b_day, b_month, b_year;

	if (sscanf(bought_date_str, "%d/%d/%d", &b_day, &b_month, &b_year) != 3 &&
		sscanf(bought_date_str, "%d-%d-%d", &b_day, &b_month, &b_year) != 3)
		return false;

	time_t t = time(NULL);
	struct tm bought_tm = { 0 };
	bought_tm.tm_mday = b_day;
	bought_tm.tm_mon = b_month - 1;
	bought_tm.tm_year = b_year - 1900;

	time_t bought_time = mktime(&bought_tm);
	double seconds = difftime(t, bought_time);
	double days = seconds / (60 * 60 * 24);

	return (days <= 14.0);
}

void process_refund(Customer* customer, TreeNode* item_to_return) 
{
	if (customer == NULL || item_to_return == NULL || customer->items_head == NULL) {
		printf("\xE2\x9D\x8C Error: Invalid refund request.\n");
		return;
	}

	ItemNode* current = customer->items_head;
	ItemNode* prev = NULL;

	while (current != NULL && current->data.item_addr->dataItem.serialNumber != item_to_return->dataItem.serialNumber) 
	{
		prev = current;
		current = current->next;
	}

	if (current == NULL) {
		printf("\xE2\x9D\x8C You have never bought this item.\n");
		return;
	}

	if (is_within_14_days(current->data.bought_date)) 
	{
		item_to_return->dataItem.stockCount++;
		item_to_return->dataItem.isInTheShop = 1;
		customer->money_spent -= item_to_return->dataItem.price;
		printf("\xE2\x9C\x85 Refund Approved! %.2f returned. The item is back in stock.\n", item_to_return->dataItem.price);
		remove_customer_item(customer, current, prev);
	}
	else
		printf("\xE2\x9D\x8C Refund Denied: More than 14 days have passed since purchase (%s).\n", current->data.bought_date);
}