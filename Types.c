#ifndef TYPES_H
#define TYPES_H
#include <stdint.h>
#define MAX_USERS 50
#define MAX_PRODUCTS 200
#define MAX_CART 50
#define USER_NAME_LEN 30
#define PRODUCT_NAME_LEN 40
#define REASON-LEN 60
#define DATE_LEN 11
#define ROLE_LEN 8
#define ROLE_ADMIN "ADMIN"
#define ROLE_USER "USER"
#define FILE_USERS "users.txt"
#define FILE_PRODUCTS "products.txt"
#define FILE_SALES "sales.txt"
#define FILE_EXPENSES "expenses.txt"
#define DIR_RECEIPTS "receipts"
#define SEP "|"

typedef struct {
	int id;
	char name[USER_NAME_LEN];
	unit32_t hash;
	char role[ROLE_LEN];
}User;

typedef struct {
	int id;
	char name[PRODUCT_NAME_LEN];
	int price;
	int quantity;
}Product;

typedef struct{
	int id;
	int userId;
	int Productid;
	int qty;
	int total;
}Sale;

typedef struct {
	int id;
	char date[DATE_LEN];
	int amount;
	char reason[REASON_LEN];
	int userId;
}Expense; 

typedef struct {
	User *user;
	char date[DATE_LEN];
	int sessionTotal;
}Session;

typedef struct{
	int productId;
	int qty;
}CartItem;

#endif
