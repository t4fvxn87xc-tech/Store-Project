
#include <stdio.h>
#include "../include/products.h"

int main(void)
{
    Product products[MAX_PRODUCTS];
    int count = 0;
    
    Product p1 = {101, "Laptop", 350000, 10};
    Product p2 = {102, "Mouse", 5000, 20};
    Product p3 = {103, "Keyboard", 12000, 15};
    
    /* Adding products */
    addProduct(products, &count, p1);
    addProduct(products, &count, p2);
    addProduct(products, &count, p3);
    
    printf("\nINITIAL PRODUCTS");
    displayProducts(products, count);
    
    /* Editing a product */
    editProduct(products, count, 101, "HP Laptop", 400000);
    
    printf("\nAFTER EDITING");
    displayProducts(products, count);
    
    /* Restocking */
    restockProduct(products, count, 102, 10);
    
    printf("\nAFTER RESTOCKING");
    displayProducts(products, count);
    
    /* Reducing stock */
    reduceStock(products, count, 103, 5);
    
    printf("\nAFTER STOCK REDUCTION");
    displayProducts(products, count);
    
    /* Deleting a product */
    deleteProduct(products, &count, 102);
    
    printf("\nAFTER DELETION");
    displayProducts(products, count);
    
    printf("\nTotal products: %d\n", count);
    
    return 0;
}
