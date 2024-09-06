#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

class Product {
public:
    string name;
    int sku;
    int stock;
    double cost;       // Cost per unit
    double costPrice;  // Cost price of the product
    int unitsSold;     // Number of units sold

    Product(string n, int s, int st, double c, double cp)
        : name(n), sku(s), stock(st), cost(c), costPrice(cp), unitsSold(0) {}

    double calculateProfitability() const {
        // Adjusted profitability considering sales, remaining stock, and cost price
        return (stock * cost);
    }

    double calculateRevenue() const {
        return unitsSold * costPrice;
    }
};

class Transaction {
public:
    int sku;
    int quantity;

    Transaction(int s, int q) : sku(s), quantity(q) {}
};

class TreeNode {
public:
    Product product;
    TreeNode* left;
    TreeNode* right;

    TreeNode(const Product& p) : product(p), left(nullptr), right(nullptr) {}
};

class InventoryManagementSystem {
private:
    TreeNode* root;
     vector<Transaction> transactionHistory;  // Transaction log

    // Helper function to insert a product into the BST
    TreeNode* insertProduct(TreeNode* node, const Product& product) {
        if (node == nullptr) {
            return new TreeNode(product);
        }

        if (product.sku < node->product.sku) {
            node->left = insertProduct(node->left, product);
        } else if (product.sku > node->product.sku) {
            node->right = insertProduct(node->right, product);
        }

        return node;
    }

    // Helper function to display products in sorted order (in-order traversal)
    void displayProducts(TreeNode* node) const {
        if (node != nullptr) {
            displayProducts(node->left);
            cout << "Name: " << node->product.name
                      << "\tSKU: " << node->product.sku
                      << "\tStock: " << node->product.stock
                      << "\tCost Price: Rs." << node->product.costPrice
                      << "\tCost: Rs." << node->product.cost
                      << "\tUnits Sold: " << node->product.unitsSold
                      << "\tRevenue: Rs." << node->product.calculateRevenue()
                      << "\tProfitability: Rs." << node->product.calculateProfitability() << "\n";
            displayProducts(node->right);
        }
    }

    // Helper function to update stock in the BST
    void updateStock(TreeNode* node, int sku, int quantity) {
        if (node != nullptr) {
            if (sku == node->product.sku) {
                // Update stock for the found product
                // Check for stock underflow
                if (quantity < 0 && node->product.stock + quantity < 0) {
                    cout << "Error: Stock underflow. Cannot decrease stock below 0.\n";
                } else {
                    // Update stock
                    node->product.stock += quantity;  // Add stock for all quantities
                    transactionHistory.emplace_back(sku, quantity);  // Log the transaction

                    // Check if stock is below 50 and display an alert
                    if (node->product.stock < 30) {
                        cout << "Alert: Stock for " << node->product.name
                                  << " is below 30. Current stock: " << node->product.stock << "\n";
                    }

                    cout << "Stock updated successfully.\n";
                }
            } else if (sku < node->product.sku) {
                // Search in the left subtree
                updateStock(node->left, sku, quantity);
            } else {
                // Search in the right subtree
                updateStock(node->right, sku, quantity);
            }
        } else {
            // Product with the specified SKU not found
            cout << "Product with SKU " << sku << " not found.\n";
        }
    }

    // Helper function to recommend stock adjustments based on revenue and sales
    void recommendStockAdjustments(TreeNode* node) const {
        if (node != nullptr) {
            recommendStockAdjustments(node->left);

            double revenue = node->product.calculateRevenue();
            int unitsSold = node->product.unitsSold;

            if (revenue > 10000) {
                cout << "Recommend stocking more of " << node->product.name
                          << ". Revenue: Rs." << revenue << "\n";
            } else if (revenue < 4500) {
                cout << "Recommend stocking less of " << node->product.name
                          << ". Revenue: Rs." << revenue << "\n";
            }

            if (unitsSold > 50) {
                cout << "Product " << node->product.name << " is selling well. Consider promoting it.\n";
            } else if (unitsSold < 10) {
                cout << "Product " << node->product.name << " has lower sales. Evaluate marketing strategies.\n";
            }

            recommendStockAdjustments(node->right);
        }
    }

public:
    InventoryManagementSystem() : root(nullptr) {}

    // Function to pre-add some products
    void preAddProducts() {
        // Add pre-defined products to the inventory
        addProduct(Product("Laptop", 1001, 50, 800.0, 700.0));
        addProduct(Product("Smartphone", 1002, 30, 400.0, 350.0));
        addProduct(Product("Tablet", 1003, 20, 300.0, 250.0));
    }

    void addProduct(const Product& product) {
        root = insertProduct(root, product);
    }

    void displayProducts() const {
        cout << "Product List (Sorted by SKU):\n";
        displayProducts(root);
        cout << "-----------------\n";
    }

    void updateStock(int sku, int quantity) {
        // Start the stock update from the root of the BST
        updateStock(root, sku, quantity);
    }

    void recommendStockAdjustments() const {
        cout << "Stock Recommendations:\n";
        recommendStockAdjustments(root);
        cout << "-----------------\n";
    }

    // Function to record sales for each product
    void recordSales() {
        cout << "Recording Sales for Each Product:\n";

        recordSalesForProduct(root);

        cout << "Sales recorded successfully.\n";
    }

    // Function to get transaction history
    const vector<Transaction>& getTransactionHistory() const {
        return transactionHistory;
    }

private:
    // Helper function to record sales for each product (in-order traversal)
    void recordSalesForProduct(TreeNode* node) {
        if (node != nullptr) {
            recordSalesForProduct(node->left);

            int unitsSold;
            cout << "Enter units sold for product " << node->product.name
                      << " (SKU: " << node->product.sku << "): ";
            cin >> unitsSold;

            // Check if units sold exceed available stock and display an alert
            if (unitsSold > node->product.stock) {
                cout << "Error: Units sold cannot exceed available stock.\n";
            } else {
                // Update units sold for the product
                node->product.unitsSold += unitsSold;
                updateStock(node, node->product.sku, -unitsSold);
            }

            recordSalesForProduct(node->right);
        }
    }
};

int main() {
    InventoryManagementSystem ims;
    cout<<"            SMART INVENTORY SYSTEM              !"<<endl;
    // Pre-add some products to the inventory
    ims.preAddProducts();

    while (true) {
        cout << "1. Add Product\n";
        cout << "2. Display Products\n";
        cout << "3. Update Stock\n";
        cout << "4. Recommend Stock Adjustments\n";
        cout << "5. Record Sales\n";
        cout << "6. View Transaction History\n";
        cout << "7. Exit\n";
        cout << "Enter your choice: ";

        int choice;
        cin >> choice;

        switch (choice) {
            case 1: {
                string name;
                int sku, stock;
                double cost, costPrice;

                cout << "Enter product name: ";
                cin.ignore();
                getline(cin, name);

                cout << "Enter SKU: ";
                cin >> sku;

                cout << "Enter initial stock: ";
                cin >> stock;

                cout << "Enter cost per unit: Rs.";
                cin >> cost;

                cout << "Enter cost price: Rs.";
                cin >> costPrice;

                ims.addProduct(Product(name, sku, stock, cost, costPrice));
                break;
            }
            case 2:
                ims.displayProducts();
                break;
            case 3: {
                int sku, quantity;

                cout << "Enter SKU for stock update: ";
                cin >> sku;

                cout << "Enter quantity for stock update: ";
                cin >> quantity;

                ims.updateStock(sku, quantity);
                break;
            }
            case 4:
                ims.recommendStockAdjustments();
                break;
            case 5:
                ims.recordSales();
                break;
            case 6: {
                const auto& history = ims.getTransactionHistory();
                cout << "Transaction History:\n";
                for (const auto& transaction : history) {
                    cout << "SKU: " << transaction.sku << "\tQuantity: " << transaction.quantity << "\n";
                }
                cout << "-----------------\n";
                break;
            }
            case 7:
                cout << "Exiting program.\n";
                return 0;
            default:
                cout << "Invalid choice. Try again.\n";
        }
    }

    return 0;
}