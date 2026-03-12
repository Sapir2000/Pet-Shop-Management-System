#pragma once
#include "Items.h"
#include "Workers.h"
#include "Customer.h"

void enter_to_system(Worker** workers, int* total_workers, Node** head, TreeNode** inventoryRoot);
void basic_worker_menu();
void worker_level_1(Worker** workers, int* total_workers, int current_worker, Node** head, TreeNode** inventoryRoot);
void worker_level_2and3(Worker** workers, int* total_workers, int current_worker, Node** head, TreeNode** inventoryRoot);
void menu_search(Worker* workers, int* total_workers, int current_worker, Node** head, TreeNode** inventoryRoot);
void menu_search_worker(Worker* workers, int* total_workers, int current_worker);
void menu_search_customer(Node** head, Worker* workers, int current_worker);
void Edit_Item_menu(TreeNode** inventoryRoot, char name[20]);