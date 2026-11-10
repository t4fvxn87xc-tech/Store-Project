#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "sales.h"

void initializeCart(Cart *cart)
{
    if (cart == NULL)
    {
        return;
    }

    cart->count = 0;
}

int addToCart(Cart *cart, const Product products[],
              int productCount, int productId, int quantity)
{
    int productIndex;
    int i;
    
    /* Validate the arguments */
    if (cart == NULL || products == NULL)
    {
        return 0;
    }
    
    if (productId <= 0 || quantity <= 0 || productCount <= 0)
    {
        return 0;
    }
    
    if (cart->count < 0 || cart->count > MAX_CART_ITEMS)
    {
        return 0;
    }
    
    /* Find the requested product */
    productIndex = findProductById(products, productCount, productId);
    
    if (productIndex == -1)
    {
        return 0;
    }
    
    /* Check whether the product is already in the cart */
    for (i = 0; i < cart->count; i++)
    {
        if (cart->items[i].productId == productId)
        {
            /* Check for integer overflow */
            if (cart->items[i].quantity > INT_MAX - quantity)
            {
                return 0;
            }
            
            /* Check the combined quantity against available stock */
            if (cart->items[i].quantity + quantity >
                products[productIndex].quantity)
            {
                return 0;
            }
            
            cart->items[i].quantity += quantity;
            
            return 1;
        }
    }
    
    /* Check whether the cart is full */
    if (cart->count >= MAX_CART_ITEMS)
    {
        return 0;
    }
    
    /* Check available stock */
    if (quantity > products[productIndex].quantity)
    {
        return 0;
    }
    
    /* Add the product to the cart */
    cart->items[cart->count].productId = productId;
    
    cart->items[cart->count].quantity = quantity;
    
    cart->items[cart->count].unitPrice =
    products[productIndex].price;
    
    cart->count++;
    
    return 1;
}

int removeFromCart(Cart *cart, int productId)
{
    int i;
    int position = -1;
    
    /* Validate cart */
    if (cart == NULL || productId <= 0)
    {
        return 0;
    }
    
    /* Find the product in the cart */
    for (i = 0; i < cart->count; i++)
    {
        if (cart->items[i].productId == productId)
        {
            position = i;
            break;
        }
    }
    
    /* Product does not exist in the cart */
    if (position == -1)
    {
        return 0;
    }
    
    /* Shift remaining items to the left */
    for (i = position; i < cart->count - 1; i++)
    {
        cart->items[i] = cart->items[i + 1];
    }
    
    /* Reduce cart item count */
    cart->count--;
    
    return 1;
}

void displayCart(const Cart *cart)
{
    int i;
    long long subtotal;
    
    if (cart == NULL)
    {
        printf("Invalid cart!\n");
        return;
    }
    
    if (cart->count == 0)
    {
        printf("\nYour shopping cart is empty!\n");
        return;
    }
    
    printf("\n==================== SHOPPING CART ====================\n");
    
    printf("%-12s %-12s %-15s %-15s\n",
           "PRODUCT ID", "QUANTITY", "UNIT PRICE", "SUBTOTAL");
    
    printf("--------------------------------------------------------\n");
    
    for (i = 0; i < cart->count; i++)
    {
        subtotal = cart->items[i].quantity *
        cart->items[i].unitPrice;
        
        printf("%-12d %-12d %-15lld %-15lld\n",
               cart->items[i].productId,
               cart->items[i].quantity,
               cart->items[i].unitPrice,
               subtotal);
    }
    
    printf("--------------------------------------------------------\n");
}

long long calculateCartTotal(const Cart *cart)
{
    long long total = 0;
    int i;
    
    if (cart == NULL)
    {
        return 0;
    }
    
    for (i = 0; i < cart->count; i++)
    {
        total += cart->items[i].quantity *
        cart->items[i].unitPrice;
    }
    
    return total;
}


