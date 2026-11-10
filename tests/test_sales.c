
#include <stdio.h>
#include "sales.h"

int main()
{
    Product products[4] = {
        {101, "Laptop", 350000, 10},
        {102, "Mouse", 5000, 20},
        {103, "Keyboard", 12000, 15},
        {104, "Monitor", 85000, 8}
    };
    
    int productCount = 4;
    Cart cart;
    
    printf("\n========== SALES MODULE TEST ==========\n");
    
    /* TEST 1: Initialize cart */
    initializeCart(&cart);
    
    printf("\n[TEST 1] Cart initialization\n");
    printf("Cart item count: %d\n", cart.count);
    
    /* TEST 2: Add different products */
    printf("\n[TEST 2] Adding products\n");
    
    printf("Laptop: %s\n",
           addToCart(&cart, products, productCount, 101, 2)
           ? "SUCCESS" : "FAILED");
    
    printf("Mouse: %s\n",
           addToCart(&cart, products, productCount, 102, 3)
           ? "SUCCESS" : "FAILED");
    
    printf("Monitor: %s\n",
           addToCart(&cart, products, productCount, 104, 1)
           ? "SUCCESS" : "FAILED");
    
    /* TEST 3: Add an existing product again */
    printf("\n[TEST 3] Adding more of an existing product\n");
    
    printf("Adding 2 more Mice: %s\n",
           addToCart(&cart, products, productCount, 102, 2)
           ? "SUCCESS" : "FAILED");
    
    /* TEST 4: Invalid requests */
    printf("\n[TEST 4] Invalid requests\n");
    
    printf("Nonexistent product: %s\n",
           addToCart(&cart, products, productCount, 999, 1)
           ? "UNEXPECTED SUCCESS" : "CORRECTLY REJECTED");
    
    printf("Insufficient stock: %s\n",
           addToCart(&cart, products, productCount, 103, 100)
           ? "UNEXPECTED SUCCESS" : "CORRECTLY REJECTED");
    
    printf("Zero quantity: %s\n",
           addToCart(&cart, products, productCount, 103, 0)
           ? "UNEXPECTED SUCCESS" : "CORRECTLY REJECTED");
    
    /* TEST 5: Display cart */
    printf("\n[TEST 5] Current shopping cart\n");
    displayCart(&cart);
    
    /* TEST 6: Calculate total */
    printf("\n[TEST 6] Calculate total\n");
    
    printf("Amount to pay: %lld FCFA\n",
           calculateCartTotal(&cart));
    
    /* TEST 7: Remove a product */
    printf("\n[TEST 7] Remove Monitor\n");
    
    printf("Removal result: %s\n",
           removeFromCart(&cart, 104)
           ? "SUCCESS" : "FAILED");
    
    displayCart(&cart);
    
    printf("New total: %lld FCFA\n",
           calculateCartTotal(&cart));
    
    /* TEST 8: Remove nonexistent cart item */
    printf("\n[TEST 8] Remove nonexistent product\n");
    
    printf("Removal result: %s\n",
           removeFromCart(&cart, 999)
           ? "UNEXPECTED SUCCESS" : "CORRECTLY REJECTED");
    
    printf("\n========== TEST COMPLETED ==========\n");
    
    return 0;
}
