
#include <stdio.h>
#include "../include/products.h"

int main(void)
{
    Product products[MAX_PRODUCTS];
    int count = 0;

    Product p1 = {101, "Laptop", 350000, 10};
    Product p2 = {102, "Mouse", 5000, 20};
    Product p3 = {103, "Keyboard", 12000, 15};

    addProduct(products, &count, p1);
    addProduct(products, &count, p2);
    addProduct(products, &count, p3);

    printf("Before deletion:\n");

    for (int i = 0; i < count; i++)
    {
        printf("%d - %s\n", products[i].id, products[i].name);
    }

    deleteProduct(products, &count, 102);

    printf("\nAfter deletion:\n");

    for (int i = 0; i < count; i++)
    {
        printf("%d - %s\n", products[i].id, products[i].name);
    }

    printf("\nTotal products: %d\n", count);

    return 0;
}
