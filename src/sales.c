#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
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

static int isValidDate(const char date[])
{
    int year, month, day;
    int daysInMonth[] = {
        31, 28, 31, 30, 31, 30,
        31, 31, 30, 31, 30, 31
    };

    int i;

    if (date == NULL || strlen(date) != 10)
        return 0;

    if (date[4] != '-' || date[7] != '-')
        return 0;

    for (i = 0; i < 10; i++)
    {
        if (i == 4 || i == 7)
            continue;

        if (date[i] < '0' || date[i] > '9')
            return 0;
    }

    year = (date[0] - '0') * 1000 +
           (date[1] - '0') * 100 +
           (date[2] - '0') * 10 +
           (date[3] - '0');

    month = (date[5] - '0') * 10 +
            (date[6] - '0');

    day = (date[8] - '0') * 10 +
          (date[9] - '0');

    if (year < 1 || month < 1 || month > 12)
        return 0;

    /* Check for a leap year */
    if ((year % 400 == 0) ||
        (year % 4 == 0 && year % 100 != 0))
    {
        daysInMonth[1] = 29;
    }

    if (day < 1 || day > daysInMonth[month - 1])
        return 0;

    return 1;
}

static int validateSaleRequest(
    const Cart *cart,
    const Product products[],
    int productCount,
    int saleId,
    int userId,
    const char date[],
    const char *productsFile,
    const char *salesFile,
    const char *receiptFile,
    long long *grandTotal)
{
    int i, j;
    int productIndex;
    long long subtotal;
    long long total = 0;
    
    /* Check pointers */
    if (cart == NULL || products == NULL || date == NULL ||
        productsFile == NULL || salesFile == NULL ||
        receiptFile == NULL || grandTotal == NULL)
    {
        return 0;
    }
    
    /* Check filenames */
    if (productsFile[0] == '\0' ||
        salesFile[0] == '\0' ||
        receiptFile[0] == '\0')
    {
        return 0;
    }
    
    /* Validate transaction details */
    if (cart->count <= 0 || cart->count > MAX_CART_ITEMS ||
        productCount <= 0 || productCount > MAX_PRODUCTS ||
        saleId <= 0 || userId <= 0)
    {
        return 0;
    }
    
    /* Validate date format: YYYY-MM-DD */
    if (strlen(date) != 10 || date[4] != '-' || date[7] != '-')
    {
        return 0;
    }
    
    for (i = 0; i < 10; i++)
    {
        if (i == 4 || i == 7)
            continue;
        
        if (date[i] < '0' || date[i] > '9')
            return 0;
    }
    
    /* Validate each cart item */
    for (i = 0; i < cart->count; i++)
    {
        if (cart->items[i].productId <= 0 ||
            cart->items[i].quantity <= 0 ||
            cart->items[i].unitPrice < 0)
        {
            return 0;
        }
        
        /* Find the product in inventory */
        productIndex = findProductById(
            products, productCount,
            cart->items[i].productId
        );
        
        if (productIndex == -1)
            return 0;
        
        /* Check available stock */
        if (cart->items[i].quantity >
            products[productIndex].quantity)
        {
            return 0;
        }
        
        /* Reject duplicate products in the cart */
        for (j = i + 1; j < cart->count; j++)
        {
            if (cart->items[i].productId ==
                cart->items[j].productId)
            {
                return 0;
            }
        }
        
        /* Prevent multiplication overflow */
        if (cart->items[i].unitPrice != 0 &&
            cart->items[i].quantity >
            LLONG_MAX / cart->items[i].unitPrice)
        {
            return 0;
        }
        
        subtotal = cart->items[i].quantity *
        cart->items[i].unitPrice;
        
        /* Prevent total addition overflow */
        if (subtotal > LLONG_MAX - total)
            return 0;
        
        total += subtotal;
    }
    
    *grandTotal = total;
    
    return 1;
}

static void prepareSaleRecords(
    const Cart *cart,
    Sale saleRecords[],
    int saleId,
    int userId,
    const char date[])
{
    int i;
    
    for (i = 0; i < cart->count; i++)
    {
        saleRecords[i].saleId = saleId;
        
        memcpy(saleRecords[i].date, date, DATE_LENGTH);
        
        saleRecords[i].userId = userId;
        
        saleRecords[i].productId =
        cart->items[i].productId;
        
        saleRecords[i].quantity =
        cart->items[i].quantity;
        
        saleRecords[i].unitPrice =
        cart->items[i].unitPrice;
        
        saleRecords[i].total =
        cart->items[i].quantity *
        cart->items[i].unitPrice;
    }
}

static int prepareUpdatedInventory(
    const Product products[],
    int productCount,
    const Cart *cart,
    Product updatedProducts[])
{
    int i;
    
    if (products == NULL || cart == NULL ||
        updatedProducts == NULL)
    {
        return 0;
    }
    
    if (productCount <= 0 || productCount > MAX_PRODUCTS)
        return 0;
    
    /* Copy the original inventory */
    memcpy(updatedProducts, products,
           productCount * sizeof(Product));
    
    /* Deduct the purchased quantities from the copy */
    for (i = 0; i < cart->count; i++)
    {
        if (!reduceStock(
            updatedProducts,
            productCount,
            cart->items[i].productId,
            cart->items[i].quantity))
        {
            return 0;
        }
    }
    
    return 1;
}

static int prepareSalesFile(
    const Sale saleRecords[],
    int recordCount,
    const char *salesFile,
    char tempFile[],
    size_t tempFileSize)
{
    FILE *source = NULL;
    FILE *destination = NULL;
    
    char buffer[4096];
    size_t bytesRead;
    int i;
    int result;
    
    /* Validate arguments */
    if (saleRecords == NULL || salesFile == NULL ||
        tempFile == NULL || recordCount <= 0)
    {
        return 0;
    }
    
    /* Generate temporary filename */
    result = snprintf(tempFile, tempFileSize,
                      "%s.tmp", salesFile);
    
    if (result < 0 || (size_t)result >= tempFileSize)
        return 0;
    
    /* Open existing sales file, if it exists */
    errno = 0;
    source = fopen(salesFile, "rb");
    
    if (source == NULL && errno != ENOENT)
        return 0;
    
    /* Create temporary file */
    destination = fopen(tempFile, "wb");
    
    if (destination == NULL)
    {
        if (source != NULL)
            fclose(source);
        
        return 0;
    }
    
    /* Preserve existing sales records */
    if (source != NULL)
    {
        while ((bytesRead = fread(buffer, 1,
            sizeof(buffer), source)) > 0)
        {
            if (fwrite(buffer, 1, bytesRead,
                destination) != bytesRead)
            {
                goto failure;
            }
        }
        
        if (ferror(source))
            goto failure;
        
        if (fclose(source) != 0)
        {
            source = NULL;
            goto failure;
        }
        
        source = NULL;
    }
    
    /* Append the new sale records */
    for (i = 0; i < recordCount; i++)
    {
        if (fprintf(destination,
            "%d|%s|%d|%d|%d|%lld|%lld\n",
            saleRecords[i].saleId,
            saleRecords[i].date,
            saleRecords[i].userId,
            saleRecords[i].productId,
            saleRecords[i].quantity,
            saleRecords[i].unitPrice,
            saleRecords[i].total) < 0)
        {
            goto failure;
        }
    }
    
    /* Ensure all data is written */
    if (fflush(destination) != 0)
        goto failure;
    
    if (fclose(destination) != 0)
    {
        destination = NULL;
        goto failure;
    }
    
    destination = NULL;
    
    return 1;
    
    failure:
    
    if (source != NULL)
        fclose(source);
    
    if (destination != NULL)
        fclose(destination);
    
    remove(tempFile);
    
    return 0;
}

static int prepareReceiptFile(
    const Sale saleRecords[],
    int recordCount,
    long long grandTotal,
    const char *receiptFile,
    char tempFile[],
    size_t tempFileSize)
{
    FILE *file;
    int i;
    int result;
    
    if (saleRecords == NULL || receiptFile == NULL ||
        tempFile == NULL || recordCount <= 0)
    {
        return 0;
    }
    
    /* Generate temporary receipt filename */
    result = snprintf(tempFile, tempFileSize,
                      "%s.tmp", receiptFile);
    
    if (result < 0 || (size_t)result >= tempFileSize)
        return 0;
    
    file = fopen(tempFile, "w");
    
    if (file == NULL)
        return 0;
    
    /* Receipt heading */
    if (fprintf(file,
        "================================================\n"
        "                 BYTELORD STORE\n"
        "                    SALES RECEIPT\n"
        "================================================\n"
        "Sale ID:       %d\n"
        "Date:          %s\n"
        "Cashier ID:    %d\n"
        "------------------------------------------------\n"
        "%-12s %-8s %-12s %-12s\n"
        "------------------------------------------------\n",
        saleRecords[0].saleId,
        saleRecords[0].date,
        saleRecords[0].userId,
        "PRODUCT ID", "QUANTITY",
        "UNIT PRICE", "SUBTOTAL") < 0)
    {
        goto failure;
    }
    
    /* Print purchased products */
    for (i = 0; i < recordCount; i++)
    {
        if (fprintf(file,
            "%-12d %-8d %-12lld %-12lld\n",
            saleRecords[i].productId,
            saleRecords[i].quantity,
            saleRecords[i].unitPrice,
            saleRecords[i].total) < 0)
        {
            goto failure;
        }
    }
    
    /* Print grand total */
    if (fprintf(file,
        "------------------------------------------------\n"
        "GRAND TOTAL: %lld FCFA\n"
        "================================================\n"
        "          THANK YOU FOR SHOPPING!\n"
        "================================================\n",
        grandTotal) < 0)
    {
        goto failure;
    }
    
    if (fflush(file) != 0)
        goto failure;
    
    if (fclose(file) != 0)
    {
        file = NULL;
        goto failure;
    }
    
    return 1;
    
    failure:
    
    if (file != NULL)
        fclose(file);
    
    remove(tempFile);
    
    return 0;
}

static int prepareProductsFile(
    const Product updatedProducts[],
    int productCount,
    const char *productsFile,
    char tempFile[],
    size_t tempFileSize)
{
    int result;
    
    if (updatedProducts == NULL || productsFile == NULL ||
        tempFile == NULL || productCount <= 0)
    {
        return 0;
    }
    
    /* Generate temporary filename */
    result = snprintf(tempFile, tempFileSize,
                      "%s.tmp", productsFile);
    
    if (result < 0 || (size_t)result >= tempFileSize)
        return 0;
    
    /* Save updated inventory into temporary file */
    if (!saveProducts(updatedProducts, productCount, tempFile))
    {
        remove(tempFile);
        return 0;
    }
    
    return 1;
}

static int checkFileExists(const char *filename)
{
    FILE *file;
    
    errno = 0;
    file = fopen(filename, "rb");
    
    if (file != NULL)
    {
        fclose(file);
        return 1;
    }
    
    if (errno == ENOENT)
        return 0;
    
    return -1;
}

static int commitSaleFiles(
    const char *productsFile,
    const char *salesFile,
    const char *receiptFile,
    const char *productsTemp,
    const char *salesTemp,
    const char *receiptTemp)
{
    char productsBackup[512];
    char salesBackup[512];
    
    int productsStatus;
    int salesStatus;
    int receiptStatus;
    int backupStatus;
    
    int productsBackedUp = 0;
    int salesBackedUp = 0;
    
    int productsInstalled = 0;
    int salesInstalled = 0;
    int receiptInstalled = 0;
    
    int result;
    
    /* Generate backup filenames */
    result = snprintf(productsBackup, sizeof(productsBackup),
                      "%s.bak", productsFile);
    
    if (result < 0 || (size_t)result >= sizeof(productsBackup))
        return 0;
    
    result = snprintf(salesBackup, sizeof(salesBackup),
                      "%s.bak", salesFile);
    
    if (result < 0 || (size_t)result >= sizeof(salesBackup))
        return 0;
    
    /* Check original files */
    productsStatus = checkFileExists(productsFile);
    salesStatus = checkFileExists(salesFile);
    receiptStatus = checkFileExists(receiptFile);
    
    if (productsStatus == -1 || salesStatus == -1 ||
        receiptStatus == -1)
        return 0;
    
    /* Never overwrite an existing receipt */
    if (receiptStatus == 1)
    {
        printf("\nReceipt already exists!\n");
        return 0;
    }
    
    /* Avoid overwriting existing backups */
    backupStatus = checkFileExists(productsBackup);
    
    if (backupStatus != 0)
        return 0;
    
    backupStatus = checkFileExists(salesBackup);
    
    if (backupStatus != 0)
        return 0;
    
    /* Back up the original inventory */
    if (productsStatus == 1)
    {
        if (rename(productsFile, productsBackup) != 0)
            goto rollback;
        
        productsBackedUp = 1;
    }
    
    /* Back up the original sales history */
    if (salesStatus == 1)
    {
        if (rename(salesFile, salesBackup) != 0)
            goto rollback;
        
        salesBackedUp = 1;
    }
    
    /* Install updated inventory */
    if (rename(productsTemp, productsFile) != 0)
        goto rollback;
    
    productsInstalled = 1;
    
    /* Install updated sales history */
    if (rename(salesTemp, salesFile) != 0)
        goto rollback;
    
    salesInstalled = 1;
    
    /* Install the receipt */
    if (rename(receiptTemp, receiptFile) != 0)
        goto rollback;
    
    receiptInstalled = 1;
    
    /* Transaction successful: remove backups */
    if (productsBackedUp)
        remove(productsBackup);
    
    if (salesBackedUp)
        remove(salesBackup);
    
    return 1;
    
    rollback:
    
    printf("\nFile installation failed. Attempting rollback...\n");
    
    if (receiptInstalled)
        remove(receiptFile);
    
    if (salesInstalled)
        remove(salesFile);
    
    if (productsInstalled)
        remove(productsFile);
    
    if (salesBackedUp)
    {
        if (rename(salesBackup, salesFile) != 0)
            printf("WARNING: Could not restore sales file!\n");
    }
    
    if (productsBackedUp)
    {
        if (rename(productsBackup, productsFile) != 0)
            printf("WARNING: Could not restore products file!\n");
    }
    
    return 0;
}

static int saleIdExists(const char *salesFile, int saleId)
{
    FILE *file;
    char line[256];
    int existingId;
    
    errno = 0;
    file = fopen(salesFile, "r");
    
    if (file == NULL)
    {
        if (errno == ENOENT)
            return 0;
        
        return -1;
    }
    
    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (sscanf(line, "%d|", &existingId) == 1)
        {
            if (existingId == saleId)
            {
                fclose(file);
                return 1;
            }
        }
    }
    
    if (ferror(file))
    {
        fclose(file);
        return -1;
    }
    
    fclose(file);
    return 0;
}

int processSale(Cart *cart, Product products[], int productCount,
                int saleId, int userId, const char date[],
                const char *productsFile, const char *salesFile,
                const char *receiptFile)
{
    Sale saleRecords[MAX_CART_ITEMS];
    Product updatedProducts[MAX_PRODUCTS];
    
    char productsTemp[512] = "";
    char salesTemp[512] = "";
    char receiptTemp[512] = "";
    
    long long grandTotal;
    
    int idStatus;
    
    int productsPrepared = 0;
    int salesPrepared = 0;
    int receiptPrepared = 0;
    
    /* 1. Validate the sale request */
    
    if (!validateSaleRequest(
        cart, products, productCount,
        saleId, userId, date,
        productsFile, salesFile, receiptFile,
        &grandTotal))
    {
        printf("\nInvalid sale request!\n");
        return 0;
    }
    
    /* 2. Check whether the sale ID already exists */
    
    idStatus = saleIdExists(salesFile, saleId);
    
    if (idStatus != 0)
    {
        if (idStatus == 1)
            printf("\nSale ID already exists!\n");
        else
            printf("\nCould not access the sales file!\n");
        
        return 0;
    }
    
    /* 3. Prepare sale records */
    
    prepareSaleRecords(
        cart, saleRecords,
        saleId, userId, date
    );
    
    /* 4. Prepare updated inventory */
    
    if (!prepareUpdatedInventory(
        products, productCount,
        cart, updatedProducts))
    {
        printf("\nCould not prepare updated inventory!\n");
        return 0;
    }
    
    /* 5. Prepare temporary products file */
    
    if (!prepareProductsFile(
        updatedProducts, productCount,
        productsFile, productsTemp,
        sizeof(productsTemp)))
    {
        printf("\nCould not prepare products file!\n");
        goto cleanup;
    }
    
    productsPrepared = 1;
    
    /* 6. Prepare temporary sales file */
    
    if (!prepareSalesFile(
        saleRecords, cart->count,
        salesFile, salesTemp,
        sizeof(salesTemp)))
    {
        printf("\nCould not prepare sales file!\n");
        goto cleanup;
    }
    
    salesPrepared = 1;
    
    /* 7. Prepare temporary receipt file */
    
    if (!prepareReceiptFile(
        saleRecords, cart->count,
        grandTotal, receiptFile,
        receiptTemp, sizeof(receiptTemp)))
    {
        printf("\nCould not prepare receipt file!\n");
        goto cleanup;
    }
    
    receiptPrepared = 1;
    
    /* 8. Install all prepared files */
    
    if (!commitSaleFiles(
        productsFile, salesFile, receiptFile,
        productsTemp, salesTemp, receiptTemp))
    {
        printf("\nSale could not be completed!\n");
        goto cleanup;
    }
    
    /* 9. Update the original inventory in memory */
    
    memcpy(
        products,
        updatedProducts,
        productCount * sizeof(Product)
    );
    
    /* 10. Empty the cart */
    
    initializeCart(cart);
    
    printf("\n================================\n");
    printf("       SALE COMPLETED!\n");
    printf("================================\n");
    
    printf("Sale ID: %d\n", saleId);
    printf("Total amount: %lld FCFA\n", grandTotal);
    printf("Receipt: %s\n", receiptFile);
    
    return 1;
    
    cleanup:
    
    if (productsPrepared)
        remove(productsTemp);
    
    if (salesPrepared)
        remove(salesTemp);
    
    if (receiptPrepared)
        remove(receiptTemp);
    
    printf("\nOriginal inventory and cart were not changed.\n");
    
    return 0;
}

/* Load saved sales records from a file */
int loadSales(Sale sales[], int *count, const char *filename)
{
    FILE *file;
    char line[256];
    Sale temp;
    char extra;

    if (sales == NULL || count == NULL || filename == NULL)
        return 0;

    *count = 0;

    file = fopen(filename, "r");

    if (file == NULL)
        return 0;

    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (sscanf(line, "%d|%10[^|]|%d|%d|%d|%lld|%lld %c",
                   &temp.saleId,
                   temp.date,
                   &temp.userId,
                   &temp.productId,
                   &temp.quantity,
                   &temp.unitPrice,
                   &temp.total,
                   &extra) != 7)
            continue;

        if (temp.saleId <= 0 ||
            temp.userId <= 0 ||
            temp.productId <= 0 ||
            temp.quantity <= 0 ||
            temp.unitPrice < 0 ||
            temp.quantity > LLONG_MAX / (temp.unitPrice == 0 ? 1 : temp.unitPrice) ||
            temp.total != temp.quantity * temp.unitPrice ||
            !isValidDate(temp.date))
            continue;

        if (*count >= MAX_SALES)
        {
            fclose(file);
            return 0;
        }

        sales[*count] = temp;
        (*count)++;
    }

    if (ferror(file))
    {
        fclose(file);
        return 0;
    }

    fclose(file);
    return 1;
}

/* Display saved sales records */
void displaySales(const Sale sales[], int count)
{
    int i;

    if (sales == NULL || count <= 0)
    {
        printf("\nNo sales records available.\n");
        return;
    }

    printf("\n================ SALES HISTORY ================\n");
    printf("%-8s %-12s %-8s %-10s %-8s %-12s %-12s\n",
           "SALE ID", "DATE", "USER ID", "PRODUCT", "QTY",
           "UNIT PRICE", "TOTAL");

    printf("--------------------------------------------------------------------------\n");

    for (i = 0; i < count; i++)
    {
        printf("%-8d %-12s %-8d %-10d %-8d %-12lld %-12lld\n",
               sales[i].saleId,
               sales[i].date,
               sales[i].userId,
               sales[i].productId,
               sales[i].quantity,
               sales[i].unitPrice,
               sales[i].total);
    }

    printf("================================================\n");
}
