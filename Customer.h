#pragma once
#include "Items.h"

typedef struct Customer_Items
{
	TreeNode* item_addr;
	char bought_date[30];
} Customer_Items;

typedef struct item_node
{
	Customer_Items data;
	struct item_node* next;
} ItemNode;

typedef struct customer
{
	char user_name[20];
	char join_date[20];
	unsigned int items_count;
	ItemNode* items_head;
	float money_spent;
} Customer;

typedef struct node
{
	Customer* customer_data;
	struct node* next;
} Node;

Node* loading_customer_file(TreeNode* inventoryRoot);
void save_customer_file(Node* head);
void free_list(Node* head);
Node* createNode(Customer* customer);
void display_customer(Node* customer);
void display_customer_items(Node* customer);
Node* find_customer(Node* head, char user_name[], Node** prev);
void add_customer(Node** head, char current_worker_name[]);
void add_customer_to_list(Node** head, char user_name[]);
void remove_customer(Node** head, Node* prev, char current_worker_name[]);
Node* enter_customer(Node* head);
ItemNode* find_customer_item(ItemNode* head, TreeNode* item_addr, ItemNode** prev);
void remove_customer_item(Customer* customer, ItemNode* item_to_remove, ItemNode* prev);
bool is_within_14_days(char* bought_date_str);
void process_refund(Customer* customer, TreeNode* item_to_return);