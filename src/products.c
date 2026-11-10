
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
