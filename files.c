#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "files.h"

#define LINE_LEN 512
#define MAX_FIELDS 8
static void copyStr(char *dst, size_t size, const char *src)
{
    strncpy(dst, src, size - 1);
    dst[size - 1] = '\0';
}


static int splitLine(char *line, char *fields[], int maxFields)
{
    int n = 0;
    char *p;

    line[strcspn(line, "\r\n")] = '\0'; 
    if (line[0] == '\0')
        return 0;

    fields[n++] = line;
    for (p = line; *p != '\0' && n < maxFields; p++) {
        if (*p == SEP) {
            *p = '\0';
            fields[n++] = p + 1;
        }
    }
    return n;
}

int fileExists(const char *path)
{
    FILE *f = fopen(path, "r");
    if (f == NULL)
        return 0;
    fclose(f);
    return 1;
}



int loadUsers(User users[], int max)
{
    FILE *f = fopen(FILE_USERS, "r");
    char line[LINE_LEN];
    char *fld[MAX_FIELDS];
    int count = 0;

    if (f == NULL)
        return 0;                         

    while (count < max && fgets(line, sizeof line, f) != NULL) {
        if (splitLine(line, fld, MAX_FIELDS) != 4)
            continue;                     
        users[count].id = atoi(fld[0]);
        copyStr(users[count].name, USER_NAME_LEN, fld[1]);
        users[count].hash = (uint32_t)strtoul(fld[2], NULL, 10);
        copyStr(users[count].role, ROLE_LEN, fld[3]);
        count++;
    }

    fclose(f);
    return count;
}

int saveUsers(const User users[], int count)
{
    FILE *f = fopen(FILE_USERS, "w");
    int i;

    if (f == NULL)
        return -1;

    for (i = 0; i < count; i++) {
        fprintf(f, "%d|%s|%lu|%s\n",
                users[i].id, users[i].name,
                (unsigned long)users[i].hash, users[i].role);
    }

    if (fclose(f) != 0)
        return -1;
    return 0;
}



int loadProducts(Product products[], int max)
{
    FILE *f = fopen(FILE_PRODUCTS, "r");
    char line[LINE_LEN];
    char *fld[MAX_FIELDS];
    int count = 0;

    if (f == NULL)
        return 0;

    while (count < max && fgets(line, sizeof line, f) != NULL) {
        if (splitLine(line, fld, MAX_FIELDS) != 4)
            continue;
        products[count].id = atoi(fld[0]);
        copyStr(products[count].name, PRODUCT_NAME_LEN, fld[1]);
        products[count].price = atoi(fld[2]);
        products[count].quantity = atoi(fld[3]);
        count++;
    }

    fclose(f);
    return count;
}

int saveProducts(const Product products[], int count)
{
    FILE *f = fopen(FILE_PRODUCTS, "w");
    int i;

    if (f == NULL)
        return -1;

    for (i = 0; i < count; i++) {
        fprintf(f, "%d|%s|%d|%d\n",
                products[i].id, products[i].name,
                products[i].price, products[i].quantity);
    }

    if (fclose(f) != 0)
        return -1;
    return 0;
}

int appendSale(const Sale *sale)
{
    FILE *f = fopen(FILE_SALES, "a");

    if (f == NULL)
        return -1;

    fprintf(f, "%d|%s|%d|%d|%d|%d\n",
            sale->id, sale->date, sale->userId,
            sale->productId, sale->qty, sale->total);

    if (fclose(f) != 0)
        return -1;
    return 0;
}

Sale *loadSales(int *count)
{
    FILE *f = fopen(FILE_SALES, "r");
    Sale *arr = NULL;
    Sale *tmp;
    int n = 0, cap = 0;
    char line[LINE_LEN];
    char *fld[MAX_FIELDS];

    *count = 0;
    if (f == NULL)
        return NULL;

    while (fgets(line, sizeof line, f) != NULL) {
        if (splitLine(line, fld, MAX_FIELDS) != 6)
            continue;

        if (n == cap) {                  
            cap = (cap == 0) ? 64 : cap * 2;
            tmp = realloc(arr, cap * sizeof(Sale));
            if (tmp == NULL) {
                free(arr);
                fclose(f);
                return NULL;
            }
            arr = tmp;
        }

        arr[n].id = atoi(fld[0]);
        copyStr(arr[n].date, DATE_LEN, fld[1]);
        arr[n].userId = atoi(fld[2]);
        arr[n].productId = atoi(fld[3]);
        arr[n].qty = atoi(fld[4]);
        arr[n].total = atoi(fld[5]);
        n++;
    }

    fclose(f);

    if (n == 0) {
        free(arr);
        return NULL;
    }
    *count = n;
    return arr;
}

int nextSaleId(void)
{
    int count, i, max = 0;
    Sale *sales = loadSales(&count);

    for (i = 0; i < count; i++) {
        if (sales[i].id > max)
            max = sales[i].id;
    }
    free(sales);                          
    return max + 1;
}

int appendExpense(const Expense *expense)
{
    FILE *f = fopen(FILE_EXPENSES, "a");

    if (f == NULL)
        return -1;

    fprintf(f, "%d|%s|%d|%s|%d\n",
            expense->id, expense->date, expense->amount,
            expense->reason, expense->userId);

    if (fclose(f) != 0)
        return -1;
    return 0;
}

Expense *loadExpenses(int *count)
{
    FILE *f = fopen(FILE_EXPENSES, "r");
    Expense *arr = NULL;
    Expense *tmp;
    int n = 0, cap = 0;
    char line[LINE_LEN];
    char *fld[MAX_FIELDS];

    *count = 0;
    if (f == NULL)
        return NULL;

    while (fgets(line, sizeof line, f) != NULL) {
        if (splitLine(line, fld, MAX_FIELDS) != 5)
            continue;

        if (n == cap) {
            cap = (cap == 0) ? 64 : cap * 2;
            tmp = realloc(arr, cap * sizeof(Expense));
            if (tmp == NULL) {
                free(arr);
                fclose(f);
                return NULL;
            }
            arr = tmp;
        }

        arr[n].id = atoi(fld[0]);
        copyStr(arr[n].date, DATE_LEN, fld[1]);
        arr[n].amount = atoi(fld[2]);
        copyStr(arr[n].reason, REASON_LEN, fld[3]);
        arr[n].userId = atoi(fld[4]);
        n++;
    }

    fclose(f);

    if (n == 0) {
        free(arr);
        return NULL;
    }
    *count = n;
    return arr;
}

int nextExpenseId(void)
{
    int count, i, max = 0;
    Expense *expenses = loadExpenses(&count);

    for (i = 0; i < count; i++) {
        if (expenses[i].id > max)
            max = expenses[i].id;
    }
    free(expenses);
    return max + 1;
}

