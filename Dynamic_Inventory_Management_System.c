#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME 100

// product structure
struct Product {
    int id;
    char name[MAX_NAME];
    float price;
    int quantity;
};

// inventory structure  
struct Inventory {
    struct Product *products;
    int count;
};

// function declarations
void addProduct(struct Inventory *inv);
void viewProducts(struct Inventory inv);
void updateQuantity(struct Inventory *inv);
void searchByID(struct Inventory inv);
void searchByName(struct Inventory inv);
void searchByPrice(struct Inventory inv);
void deleteProduct(struct Inventory *inv);
void showProduct(struct Product p);

// function to add new product
void addProduct(struct Inventory *inv) {
    inv->count++;
    inv->products = (struct Product*)realloc(inv->products, inv->count * sizeof(struct Product));
    
    if (inv->products == NULL) {
        printf("Memory reallocation failed!\n");
        exit(1);
    }

    printf("\nEnter new product details:\n");
    printf("Product ID: ");
    scanf("%d", &inv->products[inv->count - 1].id);
    printf("Product Name: ");
    scanf(" %[^\n]", inv->products[inv->count - 1].name);
    printf("Product Price: ");
    scanf("%f", &inv->products[inv->count - 1].price);
    printf("Product Quantity: ");
    scanf("%d", &inv->products[inv->count - 1].quantity);

    printf("Product added successfully!\n");
}

// display all products
void viewProducts(struct Inventory inv) {
    int i;
    
    if (inv.count == 0) {
        printf("No products available in inventory.\n");
        return;
    }

    printf("\n--- Product List ---\n");
    for (i = 0; i < inv.count; i++) {
        showProduct(inv.products[i]);
    }
}

// update quantity
void updateQuantity(struct Inventory *inv) {
    int id, newQty;
    int found = 0;
    int i;

    printf("Enter Product ID to update: ");
    scanf("%d", &id);

    for (i = 0; i < inv->count; i++) {
        if (inv->products[i].id == id) {
            printf("Enter new quantity: ");
            scanf("%d", &newQty);
            inv->products[i].quantity = newQty;
            printf("Quantity updated successfully!\n");
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("Product with ID %d not found!\n", id);
    }
}

// search product by ID
void searchByID(struct Inventory inv) {
    int id;
    int found = 0;
    int i;

    printf("Enter Product ID to search: ");
    scanf("%d", &id);

    for (i = 0; i < inv.count; i++) {
        if (inv.products[i].id == id) {
            printf("\nProduct Found:\n");
            showProduct(inv.products[i]);
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("No product found with ID %d.\n", id);
    }
}

// search by name
void searchByName(struct Inventory inv) {
    char keyword[MAX_NAME];
    int found = 0;
    int i;

    printf("Enter part of product name to search: ");
    scanf(" %[^\n]", keyword);

    for (i = 0; i < inv.count; i++) {
        if (strstr(inv.products[i].name, keyword)) {
            showProduct(inv.products[i]);
            found = 1;
        }
    }

    if (!found) {
        printf("No products found with that name.\n");
    }
}

// search by price range
void searchByPrice(struct Inventory inv) {
    float min, max;
    int found = 0;
    int i;

    printf("Enter minimum price: ");
    scanf("%f", &min);
    printf("Enter maximum price: ");
    scanf("%f", &max);

    for (i = 0; i < inv.count; i++) {
        if (inv.products[i].price >= min && inv.products[i].price <= max) {
            showProduct(inv.products[i]);
            found = 1;
        }
    }

    if (!found) {
        printf("No products found in this price range.\n");
    }
}

// delete product by ID
void deleteProduct(struct Inventory *inv) {
    int id;
    int found = 0;
    int i, j;

    printf("Enter Product ID to delete: ");
    scanf("%d", &id);

    for (i = 0; i < inv->count; i++) {
        if (inv->products[i].id == id) {
            // shift all products after this one
            for (j = i; j < inv->count - 1; j++) {
                inv->products[j] = inv->products[j + 1];
            }
            inv->count--;
            inv->products = (struct Product*)realloc(inv->products, inv->count * sizeof(struct Product));
            printf("Product deleted successfully!\n");
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("Product with ID %d not found.\n", id);
    }
}

// display single product
void showProduct(struct Product p) {
    printf("ID: %d | Name: %s | Price: %.2f | Quantity: %d\n",
           p.id, p.name, p.price, p.quantity);
}

int main() {
    struct Inventory inv;
    int choice;
    int i;

    // get initial number of products
    printf("Enter initial number of products: ");
    scanf("%d", &inv.count);
    
    if(inv.count<=0){
        printf("Enter a valid positive number of products\n");
    }

    // allocate memory
    inv.products = (struct Product*)calloc(inv.count, sizeof(struct Product));
    if (inv.products == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }

    // get product details from user
    for (i = 0; i < inv.count; i++) {
        printf("\nEnter details for product %d:\n", i + 1);
        printf("Product ID: ");
        scanf("%d", &inv.products[i].id);
        printf("Product Name: ");
        scanf(" %[^\n]", inv.products[i].name);
        printf("Product Price: ");
        scanf("%f", &inv.products[i].price);
        printf("Product Quantity: ");
        scanf("%d", &inv.products[i].quantity);
    }

    // main menu loop
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
            case 1:
                addProduct(&inv);
                break;
            case 2:
                viewProducts(inv);
                break;
            case 3:
                updateQuantity(&inv);
                break;
            case 4:
                searchByID(inv);
                break;
            case 5:
                searchByName(inv);
                break;
            case 6:
                searchByPrice(inv);
                break;
            case 7:
                deleteProduct(&inv);
                break;
            case 8:
                free(inv.products);
                printf("Exiting program...\n");
                return 0;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (1);

    return 0;
}
