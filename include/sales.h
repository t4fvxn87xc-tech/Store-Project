
#ifndef SALES_H
#define SALES_H

#include "products.h"

#define MAX_CART_ITEMS 100
#define MAX_SALES 5000
#define DATE_LENGTH 11

typedef struct {
    int productId;
    int quantity;
    long long unitPrice;
} CartItem;

typedef struct {
    CartItem items[MAX_CART_ITEMS];
    int count;
} Cart;

typedef struct {
    int saleId;
    char date[DATE_LENGTH];
    int userId;
    int productId;
    int quantity;
    long long unitPrice;
    long long total;
} Sale;

/* Cart management */
void initializeCart(Cart *cart);

int addToCart(Cart *cart, const Product products[],
              int productCount, int productId, int quantity);

int removeFromCart(Cart *cart, int productId);

void displayCart(const Cart *cart);

long long calculateCartTotal(const Cart *cart);

/* Sales management */
int processSale(Cart *cart, Product products[], int productCount,
                int saleId, int userId, const char date[],
                const char *productsFile, const char *salesFile,
                const char *receiptFile);

int loadSales(Sale sales[], int *count, const char *filename);

void displaySales(const Sale sales[], int count);

#endif
