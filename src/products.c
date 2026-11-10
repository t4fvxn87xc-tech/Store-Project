
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "products.h"


int findProductById(const Product products[], int count, int id)
{
    int i;

    for (i = 0; i < count; i++)
    {
        if (products[i].id == id)
        {
            return i;
        }
    }

    return -1;
}


int deleteProduct(Product products[], int *count, int id)
{
    int position;
    int i;

    /* Find the product */
    position = findProductById(products, *count, id);

    /* Check if product exists */
    if (position == -1)
    {
        return 0;
    }

    /* Shift elements to the left */
    for (i = position; i < *count - 1; i++)
    {
        products[i] = products[i + 1];
    }

    /* Decrease the number of products */
    (*count)--;

    return 1;
}


int addProduct(Product products[], int *count, Product newProduct)
{
    /* Check array capacity */
    if (*count >= MAX_PRODUCTS)
    {
        return 0;
    }

    /* Validate product information */
    if (newProduct.id <= 0 ||
        newProduct.name[0] == '\0' ||
        newProduct.price < 0 ||
        newProduct.quantity < 0)
    {
        return 0;
    }

    /* Prevent duplicate product IDs */
    if (findProductById(products, *count, newProduct.id) != -1)
    {
        return 0;
    }

    /* Add the new product */
    products[*count] = newProduct;

    (*count)++;

    return 1;
}


int editProduct(Product products[], int count, int id,
                const char newName[], long long newPrice)
{
    int position;
    
    /* Search for the product */
    position = findProductById(products, count, id);
    
    /* Check if product exists */
    if (position == -1)
    {
        return 0;
    }
    
    /* Validate new information */
    if (newName == NULL || newName[0] == '\0' ||
        strlen(newName) >= PRODUCT_NAME_LENGTH ||
        newPrice < 0)
    {
        return 0;
    }
    
    /* Update product information */
    strcpy(products[position].name, newName);
    products[position].price = newPrice;
    
    return 1;
}


void displayProducts(const Product products[], int count)
{
    int i;
    
    if (count == 0)
    {
        printf("\nNo products available.\n");
        return;
    }
    
    printf("\n========== PRODUCT LIST ==========\n");
    
    printf("%-8s %-20s %-15s %-10s\n",
           "ID", "NAME", "PRICE", "QUANTITY");
    
    printf("------------------------------------------------------\n");
    
    for (i = 0; i < count; i++)
    {
        printf("%-8d %-20s %-15lld %-10d\n",
               products[i].id,
               products[i].name,
               products[i].price,
               products[i].quantity);
    }
    
    printf("------------------------------------------------------\n");
}


int restockProduct(Product products[], int count, int id, int quantity)
{
    int position;
    
    if (quantity <= 0)
    {
        return 0;
    }
    
    position = findProductById(products, count, id);
    
    if (position == -1)
    {
        return 0;
    }
    
    if (products[position].quantity > INT_MAX - quantity)
    {
        return 0;
    }
    
    products[position].quantity += quantity;
    
    return 1;
}


int reduceStock(Product products[], int count, int id, int quantity)
{
    int position;
    
    if (quantity <= 0)
    {
        return 0;
    }
    
    position = findProductById(products, count, id);
    
    if (position == -1)
    {
        return 0;
    }
    
    if (products[position].quantity < quantity)
    {
        return 0;
    }
    
    products[position].quantity -= quantity;
    
    return 1;
}


int saveProducts(const Product products[], int count, const char *filename)
{
    FILE *file;
    int i;
    
    file = fopen(filename, "w");
    
    if (file == NULL)
    {
        return 0;
    }
    
    for (i = 0; i < count; i++)
    {
        fprintf(file, "%d|%s|%lld|%d\n",
                products[i].id,
                products[i].name,
                products[i].price,
                products[i].quantity);
    }
    
    if (fclose(file) != 0)
    {
        return 0;
    }
    
    return 1;
}


int loadProducts(Product products[], int *count, const char *filename)
{
    FILE *file;
    char line[150];
    Product temp;
    int loaded = 0;
    
    *count = 0;
    
    file = fopen(filename, "r");
    
    if (file == NULL)
    {
        return 0;
    }
    
    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (sscanf(line, "%d|%39[^|]|%lld|%d",
            &temp.id,
            temp.name,
            &temp.price,
            &temp.quantity) != 4)
        {
            continue;
        }
        
        if (*count >= MAX_PRODUCTS)
        {
            break;
        }
        
        if (temp.id <= 0 || temp.name[0] == '\0' ||
            temp.price < 0 || temp.quantity < 0)
        {
            continue;
        }
        
        if (findProductById(products, *count, temp.id) != -1)
        {
            continue;
        }
        
        products[*count] = temp;
        (*count)++;
        loaded++;
    }
    
    fclose(file);
    
    return 1;
}
