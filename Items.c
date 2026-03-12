#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "Workers.h"
#include "Menu_Workers.h"
#include "Menu_Customer.h"
#include "Items.h"
#include "log_action.h"

TreeNode* createItemNode(Item newItem)
{
	TreeNode* newNode = (TreeNode*)malloc(sizeof(TreeNode));
	if (!newNode)
	{
		printf("Memory allocation failed!");
		return NULL;
	}
	newNode->dataItem = newItem;
	newNode->left = NULL;
	newNode->right = NULL;
	return newNode;
}

TreeNode* insertItem(TreeNode* root, Item newItem)
{
	if (root == NULL)
		return createItemNode(newItem);
	if (newItem.serialNumber < root->dataItem.serialNumber)
		root->left = insertItem(root->left, newItem);
	else if (newItem.serialNumber > root->dataItem.serialNumber)
		root->right = insertItem(root->right, newItem);
	else
		printf("Error, Item with serial number %ld already exists\n", newItem.serialNumber);
	return root;
}

void freeTree(TreeNode* root)
{
	if (root == NULL)
		return;
	freeTree(root->left);
	freeTree(root->right);
	free(root);
}

void SaveItemsTree(TreeNode* root, FILE* fp)
{
	if (root == NULL)
		return;
	SaveItemsTree(root->left, fp);
	fwrite(&(root->dataItem), sizeof(Item), 1, fp);
	SaveItemsTree(root->right, fp);
}

TreeNode* loadItemsTree()
{
	FILE* fp = fopen("Items.bin", "rb");
	TreeNode* root = NULL;
	Item tempItem;
	if (!fp)
	{
		return NULL;
	}
	while (fread(&tempItem, sizeof(Item), 1, fp) == 1)
		root = insertItem(root, tempItem);
	fclose(fp);
	return root;
}

void printTreeInorder(TreeNode* root)
{
	if (root == NULL)
		return;
	printTreeInorder(root->left);
	printSingleItem(root->dataItem);
	printTreeInorder(root->right);
}

TreeNode* searchItemBySerialNumber(TreeNode* root, long serialNumber)
{
	if (root == NULL || root->dataItem.serialNumber == serialNumber)
		return root;

	if (root->dataItem.serialNumber < serialNumber)
		return searchItemBySerialNumber(root->right, serialNumber);

	return searchItemBySerialNumber(root->left, serialNumber);
}

void printSingleItem(Item item)
{
	char dateStr[20] = "N/A"; // ברירת מחדל אם אין תאריך
	if (item.birthDate.year != 0)
		snprintf(dateStr, sizeof(dateStr), "%02d/%02d/%04d", item.birthDate.day, item.birthDate.month, item.birthDate.year);
	else if (item.productionDate.year != 0)
		snprintf(dateStr, sizeof(dateStr), "%02d/%02d/%04d", item.productionDate.day, item.productionDate.month, item.productionDate.year);
	printf("| SN: %-7ld | Name: %-10s | Category: %-15s | Price: %-9.2f | Stock: %-4d | On Sale: %-3s | In Shop: %-3s | Date: %-10s |\n",
		item.serialNumber,
		item.name,
		item.category,
		item.price,
		item.stockCount,
		item.onSale ? "Yes" : "No",
		item.isInTheShop ? "Yes" : "No",
		dateStr);
	printf("---------------------------------------------------------------------------------------------------------------------\n");
}

int searchByCategory(TreeNode* root, char category[])
{
	int exist = 0;
	if (root == NULL)
		return 0;
	if (searchByCategory(root->left, category) == 1)
		exist = 1;
	if (_stricmp(root->dataItem.category, category) == 0)
	{
		printSingleItem(root->dataItem);
		exist = 1;
	}
	if (searchByCategory(root->right, category) == 1)
		exist = 1;
	return exist;
}

int searchByMaxPrice(TreeNode* root, double maxPrice)
{
	int exist = 0;
	if (root == NULL) return 0;
	if (searchByMaxPrice(root->left, maxPrice) == 1)
		exist = 1;
	if (root->dataItem.price <= maxPrice)
	{
		printSingleItem(root->dataItem);
		exist = 1;
	}
	if (searchByMaxPrice(root->right, maxPrice) == 1)
		exist= 1;
	return exist;
}

int searchByMinPrice(TreeNode* root, double minPrice)
{
	int exist = 0;
	if (root == NULL) return 0;
	if (searchByMinPrice(root->left, minPrice) == 1)
		exist = 1;
	if (root->dataItem.price >= minPrice)
	{
		printSingleItem(root->dataItem);
		exist = 1;
	}
	if (searchByMinPrice(root->right, minPrice) == 1)
		exist = 1;
	return exist;
}

int searchBysale(TreeNode* root, bool isBest)
{
	int exist = 0;

	if (root == NULL) return 0;
	if (searchBysale(root->left, isBest) == 1)
		exist = 1;
	if (root->dataItem.onSale == isBest)
	{
		printSingleItem(root->dataItem);
		exist = 1;
	}
	if (searchBysale(root->right, isBest) == 1)
		exist = 1;
	return exist;
}

int searchByAvailability(TreeNode* root, bool inShop)
{
	int exist = 0;
	if (root == NULL)
		return 0;
	if (searchByAvailability(root->left, inShop) == 1)
		exist = 1;
	if (root->dataItem.isInTheShop == inShop)
	{
		printSingleItem(root->dataItem);
		exist = 1;
	}
	if (searchByAvailability(root->right, inShop) == 1)
		exist = 1;
	return exist;
}

int compareDates(Date d1, Date d2) 
{
	if (d1.year != d2.year) 
		return d1.year - d2.year;
	if (d1.month != d2.month) 
		return d1.month - d2.month;
	return d1.day - d2.day;
}

int searchByName(TreeNode* root, char name[])
{
	int exist = 0;
	if (root == NULL) return 0;
	if (searchByName(root->left, name) == 1)
		exist = 1;
	if (_stricmp(root->dataItem.name, name) == 0)
	{
		printSingleItem(root->dataItem);
		exist = 1;
	}
	if (searchByName(root->right, name) == 1)
		exist = 1;
	return exist;
}

int searchByDateRange(TreeNode* root, Date start, Date end)
{
	int exist = 0, isValid;
	Date itemDate = { 0, 0, 0 };
	if (root == NULL)
		return 0;
	if (searchByDateRange(root->left, start, end) == 1)
		exist = 1;
	if (root->dataItem.birthDate.year > 0)
		itemDate = root->dataItem.birthDate;
	else if (root->dataItem.productionDate.year > 0)
		itemDate = root->dataItem.productionDate;
	if (itemDate.year > 0)
	{
		isValid = 1;
		if (start.year > 0 && compareDates(itemDate, start) < 0) //בדיקה שתאריך הפריט לא מוקדם מידי
			isValid = 0;
		if (end.year > 0 && compareDates(itemDate, end) > 0) //בדיקה שתאריך הפריט לא מאוחר מידי
			isValid = 0;
		if (isValid == 1) //אם בתנאים שלנו
		{
			printSingleItem(root->dataItem);
			exist = 1;
		}
	}
	if (searchByDateRange(root->right, start, end) == 1)
		exist = 1;
	return exist;
}

int searchByTwoTextFields(TreeNode* root, char name[], char category[])
{
	int exist = 0, nameMatch, catMatch;
	if (root == NULL)
		return 0;
	if (searchByTwoTextFields(root->left, name, category) == 1)
		exist = 1;
	nameMatch = (strlen(name) == 0 || _stricmp(root->dataItem.name, name) == 0);
	catMatch = (strlen(category) == 0 || _stricmp(root->dataItem.category, category) == 0);
	if (nameMatch && catMatch)
	{
		printSingleItem(root->dataItem);
		exist = 1;
	}
	if (searchByTwoTextFields(root->right, name, category) == 1)
		exist = 1;
	return exist;
}

TreeNode* min_node_tree(TreeNode* node)
{
	while (node->left != NULL)
		node = node->left;
	return node;
}

TreeNode* max_node_tree(TreeNode* node)
{
	while (node->right != NULL)
		node = node->right;
	return node;
}

TreeNode* delete_item_from_store(TreeNode* root, long serialNumber)
{
	char log[120];
	
	if (root == NULL)
		return root;
	if (serialNumber < root->dataItem.serialNumber)
		root->left = delete_item_from_store(root->left, serialNumber);
	else if (serialNumber > root->dataItem.serialNumber)
		root->right = delete_item_from_store(root->right, serialNumber);
	else
	{
		if (root->left == NULL)
		{
			TreeNode* temp = root->right;
			free(root);
			return temp;
		}
		else if (root->right == NULL)
		{
			TreeNode* temp = root->left;
			free(root);
			return temp;
		}
		TreeNode* temp = min_node_tree(root->right);
		root->dataItem = temp->dataItem;
		root->right = delete_item_from_store(root->right, temp->dataItem.serialNumber);
	}
	return root;
}

int is_valid_date(int day, int month, int year)
{
	if (year < 1900 || year > 2100) return 0;
	if (month < 1 || month > 12) return 0;
	if (day < 1 || day > 31) return 0;
	if (month == 2 && day > 29) return 0;
	return 1;
}



void add_item_to_store(TreeNode** inventoryRoot, char worker_name[])
{
	Item newItem = { 0 };
	TreeNode* exist;
	char log[120];
	int tempBestSeller, tempInShop, itemType, run = 1, run2 = 1, valid_name = 0, i;

	printf("--- Add New Item ---\n");
	printf("Enter Serial Number: ");
	while (run2)
	{
		if (scanf("%ld", &newItem.serialNumber) != 1|| newItem.serialNumber<=0)
		{
			while (getchar() != '\n');
			printf("\xE2\x9A\xA0 Invalid input, enter a valid serial number: ");
		}
		else if (newItem.serialNumber >= 9999999)
			printf("\xE2\x9A\xA0 serial number is too long (maximum 7 numbers), enter a valid serial number: ");
		else if (searchItemBySerialNumber(*inventoryRoot, newItem.serialNumber) != NULL)
		{
			printf("\xE2\x9A\xA0 An item with serial number %ld already exists in the store, enter different serial number: ", newItem.serialNumber);
			run2 = 1;
		}
		else
			run2 = 0;
	}
	
	/*printf("Enter Name: ");
	scanf(" %99[^\n]", newItem.name);
	while (strlen(newItem.name) > 10)
	{
		printf("\xE2\x9A\xA0 Name is too long (maximum 10 characters), enter a valid name: ");
		scanf(" %99[^\n]", newItem.name);
	}*/

	printf("Enter Name: ");
	while (valid_name == 0)
	{
		scanf(" %99[^\n]", newItem.name);
		while (getchar() != '\n');
		if (strlen(newItem.name) > 10)
		{
			printf("\xE2\x9A\xA0 Name is too long (maximum 10 characters), enter a valid name: ");
			continue;
		}
		valid_name = 1;
		for (i = 0; newItem.name[i] != '\0'; i++)
		{
			if (!((newItem.name[i] >= 'a' && newItem.name[i] <= 'z') ||
				(newItem.name[i] >= 'A' && newItem.name[i] <= 'Z') ||
				newItem.name[i] == ' '))
			{
				valid_name = 0;
				printf("\xE2\x9A\xA0 Name can only contain letters. Enter a valid name: ");
				break;
			}
		}
	}

	printf("\nWhat type of category is this?\n"
		"1. Animal\n"
		"2. Food\n"
		"3. General Product\n"
		"\nChoice: ");

	while (scanf("%d", &itemType) != 1 || itemType < 1 || itemType > 4) {
		while (getchar() != '\n');
		printf(" \xE2\x9A\xA0 Invalid choice. Please enter a number between 1 and 3: ");
	}
	printf("\n");

	switch (itemType)
	{
	case 1:
		strcpy(newItem.category, "Animal");
		while (1) {
			printf("Enter Birth Date (DD/MM/YYYY): ");
			if (scanf("%d/%d/%d", &newItem.birthDate.day, &newItem.birthDate.month, &newItem.birthDate.year) != 3) {
				while (getchar() != '\n');
				printf("\xE2\x9D\x8C Invalid format. Use DD/MM/YYYY.\n");
			}
			else if (!is_valid_date(newItem.birthDate.day, newItem.birthDate.month, newItem.birthDate.year)) {
				printf("\xE2\x9D\x8C Impossible date!\n");
			}
			else break;
		}
		break;
	case 2:
		strcpy(newItem.category, "Food");
		while (1) {
			printf("Enter Production Date (DD/MM/YYYY): ");
			if (scanf("%d/%d/%d", &newItem.productionDate.day, &newItem.productionDate.month, &newItem.productionDate.year) != 3) {
				while (getchar() != '\n');
				printf("\xE2\x9D\x8C Invalid format. Use DD/MM/YYYY.\n");
			}
			else if (!is_valid_date(newItem.productionDate.day, newItem.productionDate.month, newItem.productionDate.year)) {
				printf("\xE2\x9D\x8C Impossible date!\n");
			}
			else break;
		}
		break;
	case 3:
		strcpy(newItem.category, "General Product");
		while (1) {
			printf("Enter Production Date (DD/MM/YYYY): ");
			if (scanf("%d/%d/%d", &newItem.productionDate.day, &newItem.productionDate.month, &newItem.productionDate.year) != 3) {
				while (getchar() != '\n');
				printf("\xE2\x9D\x8C Invalid format. Use DD/MM/YYYY.\n");
			}
			else if (!is_valid_date(newItem.productionDate.day, newItem.productionDate.month, newItem.productionDate.year)) {
				printf("\xE2\x9D\x8C Impossible date!\n");
			}
			else break;
		}
	}
	printf("Enter Price: ");
	while ((scanf("%lf", &newItem.price) != 1)|| newItem.price<0|| newItem.price>999999)
	{
		while (getchar() != '\n');
		printf("\xE2\x9D\x8C Invalid input. Enter a valid price (number between 0 - 999999): ");
	}

	printf("Enter Stock Count: ");
	while ((scanf("%d", &newItem.stockCount) != 1)|| newItem.stockCount<0|| newItem.stockCount>9999)
	{
		while (getchar() != '\n');
		printf("\xE2\x9D\x8C Invalid input. Enter stock count (number between 0 - 9999): ");
	}

	printf("Is On sale? (1 = Yes or 0 = No): ");
	while (scanf("%d", &tempBestSeller) != 1 || (tempBestSeller != 0 && tempBestSeller != 1)) 
	{
		while (getchar() != '\n');
		printf("\xE2\x9D\x8C Invalid input. Enter 1 for Yes or 0 for No: ");
	}
	newItem.onSale = (bool)tempBestSeller;

	if (newItem.stockCount > 0)
		newItem.isInTheShop = 1;
	else
		newItem.isInTheShop = 0;

	*inventoryRoot = insertItem(*inventoryRoot, newItem);
	printf("\xE2\x9C\x85 Item '%s' added successfully to the inventory!\n", newItem.name);
	snprintf(log, sizeof(log), "%s added new item: %s (SN: %ld)", worker_name, newItem.name, newItem.serialNumber);
	log_action(log);
}

void menu_search_item_advanced(TreeNode** inventoryRoot, unsigned int level, char name[20],Cart* my_cart) 
{
	int choice, isBest, run = 1, exist;
	unsigned int sn;
	char category[100];
	double maxPrice, minPrice;
	Date start, end;
	TreeNode* found_item;

	while (run)
	{
		exist = 0;
		printf("\n--- Advanced Item Search \xF0\x9F\x94\x8E ---\n"
			"1. Search by Serial Number\n"
			"2. Search by Name\n"
			"3. Search by Category\n"
			"4. Search by Name and Category\n"
			"5. Search by Maximum Price\n"
			"6. Search by Minimum Price\n"
			"7. Search for items on sale\n"
			"8. Search by date\n"
			"9. Search by availability\n"
			"10. View all items\n"
			"0. Go back\n");
		printf("\nChoice: ");

		if (scanf("%d", &choice) != 1) {
			while (getchar() != '\n');
			choice = -1;
		}

		switch (choice) 
		{
		case 1:
			printf("\nEnter item Serial Number: ");
			if (scanf("%ld", &sn) == 1)
			{
				found_item = searchItemBySerialNumber(*inventoryRoot, sn);
				if (found_item != NULL)
				{
					printf("\n--- Item Details ---\n");
					printf("\n");
					printSingleItem(found_item->dataItem);
					exist = 1;
				}
			}
			else
			{
				while (getchar() != '\n');
				printf("Invalid input.\n");
			}
			break;
		case 2:
		{
			char searchName[100];
			printf("Enter Name: ");
			scanf(" %49[^\n]", searchName);
			printf("\n--- Item Details ---\n");
			exist = searchByName(*inventoryRoot, searchName);
		}
		break;
		case 3:
		{
			char searchCat[100];
			printf("\nEnter Category to search: ");
			scanf(" %99[^\n]", searchCat);
			printf("\n--- Results ---\n");
			exist = searchByCategory(*inventoryRoot, searchCat);
		}
		break;
		case 4:
		{
			char searchName[100], searchCat[100];
			printf("\n--- Multi-Field Search (Enter '0' to skip a field) ---\n");
			printf("\nEnter Name to search: ");
			scanf(" %99[^\n]", searchName);
			if (strcmp(searchName, "0") == 0)
				strcpy(searchName, "");
			printf("Enter Category to search: ");
			scanf(" %99[^\n]", searchCat);
			if (strcmp(searchCat, "0") == 0)
				strcpy(searchCat, "");
			if (strlen(searchName) == 0 && strlen(searchCat) == 0)
			{
				printf("\n\xE2\x9D\x8C Error: You must enter at least one field to search.\n");
				exist = -1;
			}
			else
			{
				printf("\n--- Results ---\n");
				exist = searchByTwoTextFields(*inventoryRoot, searchName, searchCat);
			}
		}
		break;
		case 5:
			printf("\nEnter Maximum Price: ");
			if (scanf("%lf", &maxPrice) == 1)
			{
				printf("\n--- Items under %.2f ---\n", maxPrice);
				exist = searchByMaxPrice(*inventoryRoot, maxPrice);
			}
			else
			{
				while (getchar() != '\n'); //ניקוי הבאפר
				printf("\n\xE2\x9D\x8C Error: Invalid input. Please enter a number.\n");
				exist = -1;
			}
			break;
		case 6:
			printf("\nEnter Minimum Price: ");
			if (scanf("%lf", &minPrice) == 1)
			{
				printf("\n--- Items upper %.2f ---\n", minPrice);
				exist = searchByMinPrice(*inventoryRoot, minPrice);
			}
			else
			{
				while (getchar() != '\n'); //ניקוי הבאפר
				printf("\n\xE2\x9D\x8C Error: Invalid input. Please enter a number.\n");
				exist = -1;
			}
			break;
		case 7:
			printf("\n--- Items On Sale ---\n");
			exist = searchBysale(*inventoryRoot, 1);
			break;
		case 8:
			start.day = start.month = start.year = 0;
			end.day = end.month = end.year = 0;
			printf("\nEnter the start date (DD/MM/YYYY) or write '0/0/0' for none: ");
			scanf("%d/%d/%d", &start.day, &start.month, &start.year);
			printf("\nEnter the last date (DD/MM/YYYY) or write '0/0/0' for none: ");
			scanf("%d/%d/%d", &end.day, &end.month, &end.year);
			if (start.year == 0 && end.year == 0)
			{
				printf("\n\xE2\x9D\x8C Error: You must enter at least one date.\n");
				exist = -1;
			}
			else if (start.year > 0 && end.year > 0 && compareDates(start, end) > 0)
			{
				printf("\n\xE2\x9D\x8C Error: The end date cannot be earlier than the start date.\n");
				exist = -1;
			}
			else
			{
				printf("\n--- Results ---\n");
				exist = searchByDateRange(*inventoryRoot, start, end);
			}
			break;
		case 9:
			printf("\n--- Available Items In Stock ---\n");
			exist = searchByAvailability(*inventoryRoot, true);
			printf("----------------------------------------------------------------------------\n");
			break;
		case 10:
			printf("\n--- All Store Items ---\n");
			if (*inventoryRoot != NULL)
			{
				printf("\n");
				printTreeInorder(*inventoryRoot);
				exist = 1;
			}
			break;
		case 0:
			run = 0;
			exist = -1;
			break;
		default:
			printf("\xE2\x9A\xA0 Invalid choice\n");
			exist = -1;
			break;
		}
		if (exist == 0)
			printf("\xE2\x9D\x8C Item not found!\n\n");
		else if ((level == 1 || level == 2)&&exist==1)
			Edit_Item_menu(inventoryRoot, name);
		else if (level == 0 && (my_cart->count < 3)&& exist == 1)
			add_to_cart_menu(my_cart, *inventoryRoot, name);
		else if (level == 0&& exist== 1)
			printf("\n\xE2\x9A\xA0 Your cart is full (Maximum 3 items)!\n");
	}
}

int generateLowStockReport(TreeNode* root)
{
	if (root == NULL)
		return 0;
	if (generateLowStockReport(root->left) == 1)
		return 1;
	if (root->dataItem.stockCount <= 3)
		return 1;
	if (generateLowStockReport(root->right) == 1)
		return 1;
	return 0;
}

void PrintGenerateLowStockReport(TreeNode* root)
{
	if (root == NULL)
		return;
	PrintGenerateLowStockReport(root->left);
	if (root->dataItem.stockCount <= 3)
		printSingleItem(root->dataItem);
	PrintGenerateLowStockReport(root->right);
}