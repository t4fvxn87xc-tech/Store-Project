#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CATEGORY 50
#define MAX_DATE 11
#define FILENAME "expenses.dat"
#define MAX_EXPENSES 100

// Define the core expense structure
typedef struct {
    int id;
    char category[MAX_CATEGORY];
    float amount;
    char date[MAX_DATE]; // Format: YYYY-MM-DD
} Expense;

// Function Prototypes
int loadExpenses(Expense expenses[], int max_size);
void saveAllExpenses(Expense expenses[], int count);
void addExpense(void);
void viewExpenses(void);
void generateTotalReport(void);
void generateCategoryReport(void);

int main(void) {
    int choice;
    do {
        printf("\n=============================\n");
        printf("    EXPENSE & REPORT APP     \n");
        printf("=============================\n");
        printf("1. Add New Expense\n");
        printf("2. View All Expenses\n");
        printf("3. Generate Total Report\n");
        printf("4. Generate Category Report\n");
        printf("5. Exit\n");
        printf("-----------------------------\n");
        printf("Enter your choice (1-5): ");
        
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            while(getchar() != '\n'); // Clear input buffer
            continue;
        }

        switch (choice) {
            case 1:
                addExpense();
                break;
            case 2:
                viewExpenses();
                break;
            case 3:
                generateTotalReport();
                break;
            case 4:
                generateCategoryReport();
                break;
            case 5:
                printf("Exiting application. Goodbye!\n");
                break;
            default:
                printf("Invalid choice! Choose between 1 and 5.\n");
        }
    } while (choice != 5);

    return 0;
}

// Load expenses from binary file
int loadExpenses(Expense expenses[], int max_size) {
    FILE *file = fopen(FILENAME, "rb");
    if (file == NULL) {
        return 0; // File doesn't exist yet
    }
    
    int count = 0;
    while (count < max_size && fread(&expenses[count], sizeof(Expense), 1, file) == 1) {
        count++;
    }
    
    fclose(file);
    return count;
}

// Save the entire expense array back to the binary file
void saveAllExpenses(Expense expenses[], int count) {
    FILE *file = fopen(FILENAME, "wb");
    if (file == NULL) {
        printf("Error: Could not open file for writing.\n");
        return;
    }
    
    fwrite(expenses, sizeof(Expense), count, file);
    fclose(file);
}

// Add a new expense entry
void addExpense(void) {
    Expense expenses[MAX_EXPENSES];
    int count = loadExpenses(expenses, MAX_EXPENSES);
    
    if (count >= MAX_EXPENSES) {
        printf("Error: Expense list is full (max %d items).\n", MAX_EXPENSES);
        return;
    }
    
    Expense newExp;
    newExp.id = (count > 0) ? expenses[count - 1].id + 1 : 1;
    
    printf("\n--- ADD EXPENSE ---\n");
    printf("Enter category (e.g., Food, Transport): ");
    scanf("%49s", newExp.category);
    
    printf("Enter amount: ");
    while (scanf("%f", &newExp.amount) != 1 || newExp.amount < 0) {
        printf("Invalid amount. Enter a positive number: ");
        while(getchar() != '\n');
    }
    
    printf("Enter date (YYYY-MM-DD): ");
    scanf("%10s", newExp.date);
    
    expenses[count] = newExp;
    saveAllExpenses(expenses, count + 1);
    
    printf("Success! Expense added with ID #%d.\n", newExp.id);
}

// View all logged expenses in a formatted table
void viewExpenses(void) {
    Expense expenses[MAX_EXPENSES];
    int count = loadExpenses(expenses, MAX_EXPENSES);
    
    if (count == 0) {
        printf("\nNo expenses recorded yet.\n");
        return;
    }
    
    printf("\n==========================================\n");
    printf("              ALL EXPENSES                \n");
    printf("==========================================\n");
    printf("%-5s | %-15s | %-10s | %-10s\n", "ID", "Category", "Amount", "Date");
    printf("------------------------------------------\n");
    for (int i = 0; i < count; i++) {
        printf("%-5d | %-15s | $%-9.2f | %-10s\n", 
               expenses[i].id, expenses[i].category, expenses[i].amount, expenses[i].date);
    }
    printf("==========================================\n");
}

// Report 1: Grand Total Summary
void generateTotalReport(void) {
    Expense expenses[MAX_EXPENSES];
    int count = loadExpenses(expenses, MAX_EXPENSES);
    
    if (count == 0) {
        printf("\nNo data available to generate a report.\n");
        return;
    }
    
    float grandTotal = 0.0f;
    for (int i = 0; i < count; i++) {
        grandTotal += expenses[i].amount;
    }
    
    printf("\n==========================================\n");
    printf("             FINANCIAL REPORT             \n");
    printf("==========================================\n");
    printf("Total Recorded Transactions : %d\n", count);
    printf("Grand Total Spent           : $%.2f\n", grandTotal);
    printf("==========================================\n");
}

// Report 2: Filtered Category Report
void generateCategoryReport(void) {
    Expense expenses[MAX_EXPENSES];
    int count = loadExpenses(expenses, MAX_EXPENSES);
    
    if (count == 0) {
        printf("\nNo data available for category reports.\n");
        return;
    }
    
    char targetCategory[MAX_CATEGORY];
    printf("\nEnter category to analyze: ");
    scanf("%49s", targetCategory);
    
    float categoryTotal = 0.0f;
    int matchCount = 0;
    
    printf("\n==========================================\n");
    printf("        REPORT FOR: %s\n", targetCategory);
    printf("==========================================\n");
    for (int i = 0; i < count; i++) {
        if (strcasecmp(expenses[i].category, targetCategory) == 0) {
            printf("ID: %-3d | Date: %-10s | Amount: $%.2f\n", 
                   expenses[i].id, expenses[i].date, expenses[i].amount);
            categoryTotal += expenses[i].amount;
            matchCount++;
        }
    }
    
    if (matchCount == 0) {
        printf("No expenses found under category '%s'.\n", targetCategory);
    } else {
        printf("------------------------------------------\n");
        printf("Total matches found : %d\n", matchCount);
        printf("Total spent on %s    : $%.2f\n", targetCategory, categoryTotal);
    }
    printf("==========================================\n");
}
