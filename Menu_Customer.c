#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "log_action.h"
#include "Customer.h"
#include "Menu_Customer.h"
#include "Items.h"


void customer_enter_to_system(Node** head, TreeNode* inventoryRoot)
{
	Node* customer = enter_customer(*head);
	int choice, run = 1;
	char log[100];
	Cart my_cart;
	my_cart.count = 0;
	my_cart.total_price = 0.0;

	if (customer == NULL)
		return;

	while (run)
	{
		printf("\nWhat would you like to do?\n"
			"1. View store & add items \xF0\x9F\x9B\x92\n"
			"2. My Cart & Checkout \xF0\x9F\x92\xB3\n"
			"3. Refund \xF0\x9F\xA7\xBE\n"
			"4. My Profile \xF0\x9F\x91\xA4\n"
			"0. Cancel and exit \xF0\x9F\x9A\xAA\n");
		printf("\nChoice: ");

		if (scanf("%d", &choice) != 1)
		{
			while (getchar() != '\n');
			choice = -1;
		}

		switch (choice)
		{
		case 1:
			menu_search_item_advanced(&inventoryRoot, 0, customer->customer_data->user_name,&my_cart);
			break;
		case 2:
			checkout_menu(customer, &my_cart, inventoryRoot);
			if (my_cart.count == 0)
				run = 0;
			break;
		case 3:
			menu_refund(customer, inventoryRoot);
			break;
		case 4:
			display_customer(customer);
			break;
		case 0:
			run = 0;
			break;
		default:
			printf("Invalid choice \xE2\x9A\xA0\n");
			break;
		}
	}
	snprintf(log, sizeof(log), "The customer %s logged out", customer->customer_data->user_name);
	log_action(log);
}

void add_to_cart_menu(Cart* my_cart, TreeNode* inventoryRoot, char name[20])
{
	long serial_num;
	int current_index, i, run = 1;
	TreeNode* item_node;

	while (run)
	{
		int in_cart = 0;

		if (my_cart->count >= 3)
		{
			printf("\n\xE2\x9A\xA0 Your cart is full (Maximum 3 items)!\n");
			run = 0;
			break;
		}
		
		printf("\nEnter the Serial Number of the item you want to buy (or 0 to go back): ");
		if (scanf("%ld", &serial_num) != 1)
		{
			while (getchar() != '\n');
			printf("Invalid input.\n");
		}
		else if (serial_num == 0)
			run=0;
		else
		{
			item_node = searchItemBySerialNumber(inventoryRoot, serial_num);
			if (item_node == NULL)
				printf("\xE2\x9D\x8C Item with SN %ld not found in the store.\n", serial_num);

			else
			{
				for (i = 0; i < my_cart->count; i++)
					if (my_cart->items[i].serialNumber == serial_num)
						in_cart++;

				if (item_node->dataItem.stockCount - in_cart <= 0)
					printf("\xE2\x9D\x8C Sorry, not enough stock left for '%s'!\n", item_node->dataItem.name);

				else
				{
					current_index = my_cart->count;
					strcpy(my_cart->items[current_index].item_name, item_node->dataItem.name);
					my_cart->items[current_index].price = item_node->dataItem.price;
					my_cart->items[current_index].serialNumber = item_node->dataItem.serialNumber;
					my_cart->count++;
					my_cart->total_price += item_node->dataItem.price;
					printf("\xE2\x9C\x85 '%s' added to your cart! (Price: %.2f)\n", item_node->dataItem.name, item_node->dataItem.price);
					printf("Current Cart Total: %.2f | Items in Cart: %d/3\n", my_cart->total_price, my_cart->count);
				}
			}
		}
	}
}

void checkout_menu(Node* customer_node, Cart* my_cart, TreeNode* inventoryRoot)
{
	int i, confirm, current_stock;
	char log[120];
	char* current_date;
	Customer* customer = customer_node->customer_data;

	while (1)
	{
		if (my_cart->count == 0)
		{
			printf("\xE2\x9D\x8C Your cart is empty!\n");
			return;
		}
		printf("\n--- Current Cart Status ---\n");
		printf("\n");
		printf("%-4s | %-15s | %-8s | %-12s\n", "No.", "Item Name", "Price", "Store Stock");
		printf("--------------------------------------------------\n");
		for (i = 0; i < my_cart->count; i++)
		{
			TreeNode* item_node_display = searchItemBySerialNumber(inventoryRoot, my_cart->items[i].serialNumber);
			current_stock = (item_node_display != NULL) ? item_node_display->dataItem.stockCount : 0;
			printf("%-4d | %-15s | %-8.2f | %-12d\n",
				i + 1,
				my_cart->items[i].item_name,
				my_cart->items[i].price,
				current_stock);
		}
		printf("--------------------------------------------------\n");
		printf("Total to pay: %.2f\n", my_cart->total_price);
		printf("\n1. Confirm and Pay \xF0\x9F\x92\xB3\n"
			"2. Remove an Item (Change your mind) \xE2\x9D\x8C\n"
			"0. Back to shop\n");
		printf("\nChoice: ");

		if (scanf("%d", &confirm) != 1)
		{
			while (getchar() != '\n');
			confirm = -1;
		}

		if (confirm == 1)
		{
			current_date = organisde_current_date();
			for (i = 0; i < my_cart->count; i++)
			{
				TreeNode* item_in_tree = searchItemBySerialNumber(inventoryRoot, my_cart->items[i].serialNumber);
				if (item_in_tree != NULL)
				{
					item_in_tree->dataItem.stockCount--;
					if (item_in_tree->dataItem.stockCount < 1)
						item_in_tree->dataItem.isInTheShop = 0;
					ItemNode* new_item = (ItemNode*)malloc(sizeof(ItemNode));
					if (new_item != NULL)
					{
						new_item->data.item_addr = createItemNode(item_in_tree->dataItem);
						strcpy(new_item->data.bought_date, current_date);
						new_item->next = customer->items_head;
						customer->items_head = new_item;
						customer->items_count++;
					}
					else
					{
						printf("\xE2\x9A\xA0 Error: Failed to allocate memory for item.\n");
					}
				}
			}
			customer->money_spent += (float)my_cart->total_price;
			snprintf(log, sizeof(log), "Customer %s purchased %d items for %.2f", customer->user_name, my_cart->count, my_cart->total_price);
			log_action(log);
			printf("\xE2\x9C\x85 Payment successful! Thank you for buying.\n");
			my_cart->count = 0;
			my_cart->total_price = 0.0;
			return;
		}
		else if (confirm == 2)
			remove_from_cart(my_cart);
		else if (confirm == 0)
			return;
		else
			printf("Invalid choice. Please try again.\n");
	}
}

void menu_refund(Node* customer_node, TreeNode* inventoryRoot)
{
	unsigned int serial_num;

	display_customer_items(customer_node);
	printf("\nRefunds can only be made up to 14 days from the date of purchase.\n");
	printf("Enter the Serial Number of the item you'd like to return: ");

	if (scanf("%ld", &serial_num) != 1)
	{
		while (getchar() != '\n');
		printf("Invalid input.\n");
		return;
	}

	printf("\n");

	TreeNode* item_to_return = searchItemBySerialNumber(inventoryRoot, serial_num);
	if (item_to_return == NULL)
	{
		printf("\xE2\x9D\x8C Item not found in the store inventory.\n");
		return;
	}
	process_refund(customer_node->customer_data, item_to_return);
}

void remove_from_cart(Cart* my_cart)
{
	int index, i;
	if (my_cart->count == 0)
	{
		printf("\xE2\x9D\x8C Your cart is already empty.\n");
		return;
	}
	printf("\nWhich item would you like to remove? (1-%d): ", my_cart->count);
	if (scanf("%d", &index) != 1 || index < 1 || index > my_cart->count)
	{
		while (getchar() != '\n');
		printf("\xE2\x9A\xA0 Invalid choice.\n");
		return;
	}
	index--;
	printf("\xE2\x9C\x85 Removed: %s\n", my_cart->items[index].item_name);
	my_cart->total_price -= my_cart->items[index].price;
	for (i = index; i < my_cart->count - 1; i++)
		my_cart->items[i] = my_cart->items[i + 1];
	my_cart->count--;
	printf("Items in cart: %d/3\n", my_cart->count);
}