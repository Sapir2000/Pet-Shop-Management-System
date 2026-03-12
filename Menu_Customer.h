#pragma once
#include "Customer.h"
#include "Items.h"

typedef struct cartItem {
	char item_name[50];
	double price; 
	unsigned int serialNumber;
} CartItem;

typedef struct cart {
	CartItem items[3];
	int count;
	double total_price;
} Cart;

void customer_enter_to_system(Node** head, TreeNode* inventoryRoot);
void add_to_cart_menu(Cart* my_cart, TreeNode* inventoryRoot, char name[20]);
void checkout_menu(Node* customer_node, Cart* my_cart, TreeNode* inventoryRoot);
void menu_refund(Node* customer, TreeNode* inventoryRoot);
void remove_from_cart(Cart* my_cart);