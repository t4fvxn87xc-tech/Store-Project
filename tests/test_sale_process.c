#include <stdio.h>
#include <string.h>
#include "sales.h"

int main(void)
{
    Product products[2] = {
        {101, "Laptop", 350000, 10},
        {103, "Keyboard", 12000, 15}
    };

    Product savedProducts[MAX_PRODUCTS];
    Sale sales[MAX_SALES];

    int productCount = 2;
    int savedCount = 0;
    int salesCount = 0;

    Cart cart;

    const char *productsFile = "/tmp/bytelord_test_products.txt";
    const char *salesFile = "/tmp/bytelord_test_sales.txt";
    const char *receiptFile = "/tmp/bytelord_test_receipt.txt";

    FILE *receipt;

    printf("\n========== SALE PROCESSING INTEGRATION TEST ==========\n");

    /* Remove files from any previous test */
    remove(productsFile);
    remove(salesFile);
    remove(receiptFile);

    remove("/tmp/bytelord_test_products.txt.bak");
    remove("/tmp/bytelord_test_sales.txt.bak");

    remove("/tmp/bytelord_test_products.txt.tmp");
    remove("/tmp/bytelord_test_sales.txt.tmp");
    remove("/tmp/bytelord_test_receipt.txt.tmp");

    /* TEST 1: Save initial inventory */

    printf("\n[TEST 1] Preparing initial inventory\n");

    if (!saveProducts(products, productCount, productsFile))
    {
        printf("FAILED: Could not save initial inventory.\n");
        return 1;
    }

    printf("Initial inventory saved successfully.\n");

    /* TEST 2: Prepare shopping cart */

    printf("\n[TEST 2] Preparing shopping cart\n");

    initializeCart(&cart);

    if (!addToCart(&cart, products, productCount, 101, 2))
        return 1;

    if (!addToCart(&cart, products, productCount, 103, 3))
        return 1;

    printf("Cart total: %lld FCFA\n", calculateCartTotal(&cart));

    /* TEST 3: Process the sale */

    printf("\n[TEST 3] Processing sale\n");

    if (!processSale(
            &cart,
            products,
            productCount,
            9001,
            7,
            "2026-11-10",
            productsFile,
            salesFile,
            receiptFile))
    {
        printf("FAILED: Sale processing unsuccessful.\n");
        return 1;
    }

    printf("Sale processing successful.\n");

    /* TEST 4: Verify stock deduction */

    printf("\n[TEST 4] Checking updated inventory\n");

    if (products[0].quantity != 8 ||
        products[1].quantity != 12)
    {
        printf("FAILED: Incorrect stock quantities.\n");
        return 1;
    }

    printf("Laptop stock: %d\n", products[0].quantity);
    printf("Keyboard stock: %d\n", products[1].quantity);

    /* TEST 5: Verify cart clearing */

    printf("\n[TEST 5] Checking cart\n");

    if (cart.count != 0)
    {
        printf("FAILED: Cart was not cleared.\n");
        return 1;
    }

    printf("Cart successfully cleared.\n");

    /* TEST 6: Verify inventory persistence */

    printf("\n[TEST 6] Checking saved inventory\n");

    if (!loadProducts(savedProducts, &savedCount, productsFile))
        return 1;

    if (savedCount != 2 ||
        savedProducts[0].quantity != 8 ||
        savedProducts[1].quantity != 12)
    {
        printf("FAILED: Saved inventory is incorrect.\n");
        return 1;
    }

    printf("Saved inventory is correct.\n");

    /* TEST 7: Verify sales persistence */

    printf("\n[TEST 7] Checking saved sales records\n");

    if (!loadSales(sales, &salesCount, salesFile))
        return 1;

    if (salesCount != 2 ||
        sales[0].saleId != 9001 ||
        sales[1].saleId != 9001)
    {
        printf("FAILED: Sales records are incorrect.\n");
        return 1;
    }

    displaySales(sales, salesCount);

    /* TEST 8: Verify receipt generation */

    printf("\n[TEST 8] Checking receipt\n");

    receipt = fopen(receiptFile, "r");

    if (receipt == NULL)
    {
        printf("FAILED: Receipt was not generated.\n");
        return 1;
    }

    fclose(receipt);

    printf("Receipt generated successfully.\n");

    /* TEST 9: Reject duplicate sale ID */

    printf("\n[TEST 9] Checking duplicate sale ID protection\n");

    if (!addToCart(&cart, products, productCount, 101, 1))
        return 1;

    if (processSale(
            &cart,
            products,
            productCount,
            9001,
            7,
            "2026-11-10",
            productsFile,
            salesFile,
            receiptFile))
    {
        printf("FAILED: Duplicate sale ID was accepted.\n");
        return 1;
    }

    if (cart.count != 1 || products[0].quantity != 8)
    {
        printf("FAILED: Inventory or cart changed after rejection.\n");
        return 1;
    }

    printf("Duplicate sale correctly rejected.\n");
    printf("Cart and inventory remained unchanged.\n");

    printf("\n========== ALL INTEGRATION TESTS PASSED ==========\n");

    /* Clean up temporary test files */

    remove(productsFile);
    remove(salesFile);
    remove(receiptFile);

    return 0;
}
