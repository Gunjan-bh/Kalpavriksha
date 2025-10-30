#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_NAME_LENGTH 50
#define MAX_PRODUCTS 100
#define MAX_PRODUCT_ID 10000
#define MAX_PRICE 100000
#define MAX_QUANTITY 1000000

struct Product {
    int productID;
    char productName[MAX_NAME_LENGTH];
    float price;
    int quantity;
};

struct Inventory {
    struct Product *products;
    int productCount;
};

// Function declarations
void addProduct(struct Inventory *inventory);
void viewProducts(struct Inventory inventory);
void updateProductQuantity(struct Inventory *inventory);
void searchProductByID(struct Inventory inventory);
void searchProductByName(struct Inventory inventory);
void searchProductByPriceRange(struct Inventory inventory);
void deleteProductByID(struct Inventory *inventory);
void showProduct(struct Product product);
int isValidName(const char *name);
int isDuplicateID(struct Inventory inventory, int id, int currentIndex);
int isDuplicateName(struct Inventory inventory, const char *name, int currentIndex);
int getValidInteger(const char *message, int min, int max);
float getValidFloat(const char *message, float min, float max);

// ---------------- Validation and Utility Functions ----------------

int isValidName(const char *name) {
    if (strlen(name) == 0 || strlen(name) > MAX_NAME_LENGTH)
        return 0;

    int onlyDigits = 1;
    for (int i = 0; name[i] != '\0'; i++) {
        if (!isalpha(name[i]) && !isspace(name[i]))
            return 0;
        if (isalpha(name[i]))
            onlyDigits = 0;
    }
    return !onlyDigits;
}

int getValidInteger(const char *message, int min, int max) {
    int value;
    while (1) {
        printf("%s", message);
        if (scanf("%d", &value) != 1) {
            while (getchar() != '\n');
            printf("Invalid input. Please enter a number.\n");
        } else if (value < min || value > max) {
            printf("Value must be between %d and %d.\n", min, max);
        } else {
            return value;
        }
    }
}

float getValidFloat(const char *message, float min, float max) {
    float value;
    while (1) {
        printf("%s", message);
        if (scanf("%f", &value) != 1) {
            while (getchar() != '\n');
            printf("Invalid input. Please enter a numeric value.\n");
        } else if (value < min || value > max) {
            printf("Value must be between %.2f and %.2f.\n", min, max);
        } else {
            return value;
        }
    }
}

// Skip comparing with the current index to avoid self-duplicate detection
int isDuplicateID(struct Inventory inventory, int id, int currentIndex) {
    for (int i = 0; i < inventory.productCount; i++) {
        if (i == currentIndex) continue;
        if (inventory.products[i].productID == id)
            return 1;
    }
    return 0;
}

int isDuplicateName(struct Inventory inventory, const char *name, int currentIndex) {
    for (int i = 0; i < inventory.productCount; i++) {
        if (i == currentIndex) continue;
        if (strcasecmp(inventory.products[i].productName, name) == 0)
            return 1;
    }
    return 0;
}

// ---------------- Core Functions ----------------

void addProduct(struct Inventory *inventory) {
    if (inventory->productCount >= MAX_PRODUCTS) {
        printf("Inventory is full. Cannot add more products.\n");
        return;
    }

    struct Product newProduct;

    do {
        newProduct.productID = getValidInteger("Enter Product ID (1-10000): ", 1, MAX_PRODUCT_ID);
        if (isDuplicateID(*inventory, newProduct.productID, -1))
            printf("Error: Duplicate Product ID found. Try again.\n");
    } while (isDuplicateID(*inventory, newProduct.productID, -1));

    do {
        printf("Enter Product Name (1-50 characters): ");
        scanf(" %[^\n]", newProduct.productName);
        if (!isValidName(newProduct.productName))
            printf("Invalid name. Use only alphabets and spaces.\n");
        else if (isDuplicateName(*inventory, newProduct.productName, -1))
            printf("Error: Duplicate Product Name found. Try again.\n");
    } while (!isValidName(newProduct.productName) || isDuplicateName(*inventory, newProduct.productName, -1));

    newProduct.price = getValidFloat("Enter Product Price (0-100000): ", 0, MAX_PRICE);
    newProduct.quantity = getValidInteger("Enter Product Quantity (0-1000000): ", 0, MAX_QUANTITY);

    inventory->productCount++;
    inventory->products = realloc(inventory->products, inventory->productCount * sizeof(struct Product));
    if (inventory->products == NULL) {
        printf("Memory allocation failed!\n");
        exit(1);
    }

    inventory->products[inventory->productCount - 1] = newProduct;

    printf("✅ Product added successfully!\n");
}

void viewProducts(struct Inventory inventory) {
    if (inventory.productCount == 0) {
        printf("No products available in inventory.\n");
        return;
    }
    printf("\n--- Product List ---\n");
    for (int i = 0; i < inventory.productCount; i++) {
        showProduct(inventory.products[i]);
    }
}

void updateProductQuantity(struct Inventory *inventory) {
    int id = getValidInteger("Enter Product ID to update quantity: ", 1, MAX_PRODUCT_ID);
    for (int i = 0; i < inventory->productCount; i++) {
        if (inventory->products[i].productID == id) {
            inventory->products[i].quantity = getValidInteger("Enter new quantity (0-1000000): ", 0, MAX_QUANTITY);
            printf("Quantity updated successfully!\n");
            return;
        }
    }
    printf("Product with ID %d not found.\n", id);
}

void searchProductByID(struct Inventory inventory) {
    int id = getValidInteger("Enter Product ID to search: ", 1, MAX_PRODUCT_ID);
    for (int i = 0; i < inventory.productCount; i++) {
        if (inventory.products[i].productID == id) {
            showProduct(inventory.products[i]);
            return;
        }
    }
    printf("No product found with ID %d.\n", id);
}

void searchProductByName(struct Inventory inventory) {
    char keyword[MAX_NAME_LENGTH];
    printf("Enter product name or part of it: ");
    scanf(" %[^\n]", keyword);

    int found = 0;
    for (int i = 0; i < inventory.productCount; i++) {
        char lowerName[MAX_NAME_LENGTH], lowerKey[MAX_NAME_LENGTH];
        strcpy(lowerName, inventory.products[i].productName);
        strcpy(lowerKey, keyword);

        for (int j = 0; lowerName[j]; j++) lowerName[j] = tolower(lowerName[j]);
        for (int j = 0; lowerKey[j]; j++) lowerKey[j] = tolower(lowerKey[j]);

        if (strstr(lowerName, lowerKey)) {
            showProduct(inventory.products[i]);
            found = 1;
        }
    }

    if (!found)
        printf("No products found with that name.\n");
}

void searchProductByPriceRange(struct Inventory inventory) {
    float min = getValidFloat("Enter minimum price: ", 0, MAX_PRICE);
    float max = getValidFloat("Enter maximum price: ", min, MAX_PRICE);

    int found = 0;
    for (int i = 0; i < inventory.productCount; i++) {
        if (inventory.products[i].price >= min && inventory.products[i].price <= max) {
            showProduct(inventory.products[i]);
            found = 1;
        }
    }
    if (!found)
        printf("No products found in this price range.\n");
}

void deleteProductByID(struct Inventory *inventory) {
    int id = getValidInteger("Enter Product ID to delete: ", 1, MAX_PRODUCT_ID);

    for (int i = 0; i < inventory->productCount; i++) {
        if (inventory->products[i].productID == id) {
            for (int j = i; j < inventory->productCount - 1; j++) {
                inventory->products[j] = inventory->products[j + 1];
            }
            inventory->productCount--;
            inventory->products = realloc(inventory->products, inventory->productCount * sizeof(struct Product));
            printf("Product deleted successfully!\n");
            return;
        }
    }
    printf("Product with ID %d not found.\n", id);
}

void showProduct(struct Product product) {
    printf("ID: %d | Name: %s | Price: %.2f | Quantity: %d\n",
           product.productID, product.productName, product.price, product.quantity);
}

// ---------------- Main Function ----------------

int main() {
    struct Inventory inventory = {NULL, 0};
    int choice;

    inventory.productCount = getValidInteger("Enter initial number of products (1-100): ", 1, MAX_PRODUCTS);

    inventory.products = calloc(inventory.productCount, sizeof(struct Product));
    if (inventory.products == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    for (int i = 0; i < inventory.productCount; i++) {
        printf("\nEnter details for product %d:\n", i + 1);

        do {
            inventory.products[i].productID = getValidInteger("Product ID (1-10000): ", 1, MAX_PRODUCT_ID);
            if (isDuplicateID(inventory, inventory.products[i].productID, i))
                printf("Duplicate ID found! Try again.\n");
        } while (isDuplicateID(inventory, inventory.products[i].productID, i));

        do {
            printf("Product Name: ");
            scanf(" %[^\n]", inventory.products[i].productName);
            if (!isValidName(inventory.products[i].productName))
                printf("Invalid name. Use only letters and spaces.\n");
            else if (isDuplicateName(inventory, inventory.products[i].productName, i))
                printf("Duplicate name found! Try again.\n");
        } while (!isValidName(inventory.products[i].productName) ||
                 isDuplicateName(inventory, inventory.products[i].productName, i));

        inventory.products[i].price = getValidFloat("Price (0-100000): ", 0, MAX_PRICE);
        inventory.products[i].quantity = getValidInteger("Quantity (0-1000000): ", 0, MAX_QUANTITY);
    }

    do {
        printf("\n--- Inventory Management System ---\n");
        printf("1. Add Product\n");
        printf("2. View All Products\n");
        printf("3. Update Product Quantity\n");
        printf("4. Search Product by ID\n");
        printf("5. Search Product by Name\n");
        printf("6. Search Products by Price Range\n");
        printf("7. Delete Product by ID\n");
        printf("8. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: addProduct(&inventory); break;
            case 2: viewProducts(inventory); break;
            case 3: updateProductQuantity(&inventory); break;
            case 4: searchProductByID(inventory); break;
            case 5: searchProductByName(inventory); break;
            case 6: searchProductByPriceRange(inventory); break;
            case 7: deleteProductByID(&inventory); break;
            case 8:
                free(inventory.products);
                printf("Exiting program. Memory freed successfully.\n");
                return 0;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (1);

    return 0;
}
