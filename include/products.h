
#ifndef PRODUCTS_H
#define PRODUCTS_H

#define MAX_PRODUCTS 500
#define PRODUCT_NAME_LENGTH 40

typedef struct {
    int id;
    char name[PRODUCT_NAME_LENGTH];
    long long price;
    int quantity;
} Product;

/* Product management */
int addProduct(Product products[], int *count, Product newProduct);

int editProduct(Product products[], int count, int id,
                const char newName[], long long newPrice);

int deleteProduct(Product products[], int *count, int id);

/* Product searching and display */
int findProductById(const Product products[], int count, int id);

void displayProducts(const Product products[], int count);

/* Stock management */
int restockProduct(Product products[], int count, int id, int quantity);

int reduceStock(Product products[], int count, int id, int quantity);

/* File management */
int loadProducts(Product products[], int *count, const char *filename);

int saveProducts(const Product products[], int count, const char *filename);

#endif
