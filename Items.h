#pragma once
#include <stdio.h>
#include <stdbool.h>

typedef struct cart Cart;

typedef struct date {
	int day;
	int month;
	int year;
} Date;

typedef struct item {
	long serialNumber;
	char name[100];
	char category[100];
	double price;
	bool onSale;
	bool isInTheShop;        
	Date productionDate;
	Date birthDate;
	int stockCount;
} Item;

typedef struct treeNode {
	Item dataItem;
	struct treeNode* left;
	struct treeNode* right;
} TreeNode;

TreeNode* createItemNode(Item newItem);
TreeNode* insertItem(TreeNode* root, Item newItem);
void freeTree(TreeNode* root);
void SaveItemsTree(TreeNode* root, FILE* fp);
TreeNode* loadItemsTree();
void printTreeInorder(TreeNode* root);
int compareDates(Date d1, Date d2);
TreeNode* min_node_tree(TreeNode* node);
TreeNode* max_node_tree(TreeNode* node);
TreeNode* delete_item_from_store(TreeNode* root, long serialNumber); //לשאול את ולדי על ההיסטוריה של הפריט

TreeNode* searchItemBySerialNumber(TreeNode* root, long serialNumber);
void printSingleItem(Item item);
int searchByCategory(TreeNode* root, char category[]);
int searchByMaxPrice(TreeNode* root, double maxPrice);
int searchByMinPrice(TreeNode* root, double minPrice);
int searchBysale(TreeNode* root, bool isBest);
int searchByAvailability(TreeNode* root, bool inShop);
int searchByName(TreeNode* root, char name[]);
int searchByDateRange(TreeNode* root, Date start, Date end);
int searchByTwoTextFields(TreeNode* root, char name[], char category[]);
int is_valid_date(int day, int month, int year);
int generateLowStockReport(TreeNode* root);
void PrintGenerateLowStockReport(TreeNode* root);

void menu_search_item_advanced(TreeNode** inventoryRoot, unsigned int level, char name[20],Cart* my_cart);
void add_item_to_store(TreeNode** inventoryRoot, char worker_name[]);