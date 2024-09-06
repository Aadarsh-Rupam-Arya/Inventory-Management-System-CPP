#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>  // For file handling
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
        return (stock * cost);
    }

    double calculateRevenue() const {
        return unitsSold * costPrice;
    }

    // Save product data to a file
    void saveProduct(ofstream& file) const {
        file << name << "," << sku << "," << stock << "," << cost << "," << costPrice << "," << unitsSold << "\n";
    }

    // Load product data from a file
    static Product loadProduct(ifstream& file) {
        string name;
        int sku, stock, unitsSold;
        double cost, costPrice;
        char comma;

        getline(file, name, ',');
        file >> sku >> comma >> stock >> comma >> cost >> comma >> costPrice >> comma >> unitsSold;
        file.ignore();  // To skip the newline character

        return Product(name, sku, stock, cost, costPrice);
    }
};

class Transaction {
public:
    int sku;
    int quantity;

    Transaction(int s, int q) : sku(s), quantity(q) {}

    // Save transaction data to a file
    void saveTransaction(ofstream& file) const {
        file << sku << "," << quantity << "\n";
    }

    // Load transaction data from a file
    static Transaction loadTransaction(ifstream& file) {
        int sku, quantity;
        char comma;

        file >> sku >> comma >> quantity;
        file.ignore();  // To skip the newline character

        return Transaction(sku, quantity);
    }
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
    const string productFile = "products.txt";
    const string transactionFile = "transactions.txt";

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
                if (quantity < 0 && node->product.stock + quantity < 0) {
                    cout << "Error: Stock underflow. Cannot decrease stock below 0.\n";
                } else {
                    node->product.stock += quantity;
                    transactionHistory.emplace_back(sku, quantity);  // Log the transaction

                    if (node->product.stock < 30) {
                        cout << "Alert: Stock for " << node->product.name
                                  << " is below 30. Current stock: " << node->product.stock << "\n";
                    }

                    cout << "Stock updated successfully.\n";
                }
            } else if (sku < node->product.sku) {
                updateStock(node->left, sku, quantity);
            } else {
                updateStock(node->right, sku, quantity);
            }
        } else {
            cout << "Product with SKU " << sku << " not found.\n";
        }
    }

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

    void addProduct(const Product& product) {
        root = insertProduct(root, product);
    }

    void preAddProducts() {
    addProduct(Product("SmartPhone", 1001, 100, 150.00, 180.00));
    addProduct(Product("Keyboard", 1002, 200, 250.00, 290.00));
    addProduct(Product("Mouse", 1003, 100, 250.00, 190.00));

}

    void displayProducts() const {
        cout << "Product List (Sorted by SKU):\n";
        displayProducts(root);
        cout << "-----------------\n";
    }

    void updateStock(int sku, int quantity) {
        updateStock(root, sku, quantity);
    }

    void recommendStockAdjustments() const {
        cout << "Stock Recommendations:\n";
        recommendStockAdjustments(root);
        cout << "-----------------\n";
    }

    void recordSales() {
        cout << "Recording Sales for Each Product:\n";
        recordSalesForProduct(root);
        cout << "Sales recorded successfully.\n";
    }

    const vector<Transaction>& getTransactionHistory() const {
        return transactionHistory;
    }

    // Function to save all products to a file
    void saveProducts() const {
        ofstream file(productFile);
        if (!file) {
            cout << "Error: Could not open file for saving products.\n";
            return;
        }

        saveProductsToFile(root, file);
        file.close();
    }

    // Helper function to save products to file (in-order traversal)
    void saveProductsToFile(TreeNode* node, ofstream& file) const {
        if (node != nullptr) {
            saveProductsToFile(node->left, file);
            node->product.saveProduct(file);
            saveProductsToFile(node->right, file);
        }
    }

    // Function to load products from a file
    void loadProducts() {
        ifstream file(productFile);
        if (!file) {
            cout << "Error: Could not open file for loading products.\n";
            return;
        }

        while (file.peek() != EOF) {
            Product product = Product::loadProduct(file);
            addProduct(product);
        }

        file.close();
    }

    // Function to save transaction history to a file
    void saveTransactions() const {
        ofstream file(transactionFile);
        if (!file) {
            cout << "Error: Could not open file for saving transactions.\n";
            return;
        }

        for (const auto& transaction : transactionHistory) {
            transaction.saveTransaction(file);
        }

        file.close();
    }

    // Function to load transaction history from a file
    void loadTransactions() {
        ifstream file(transactionFile);
        if (!file) {
            cout << "Error: Could not open file for loading transactions.\n";
            return;
        }

        while (file.peek() != EOF) {
            Transaction transaction = Transaction::loadTransaction(file);
            transactionHistory.push_back(transaction);
        }

        file.close();
    }

private:
    void recordSalesForProduct(TreeNode* node) {
        if (node != nullptr) {
            recordSalesForProduct(node->left);

            int unitsSold;
            cout << "Enter units sold for product " << node->product.name
                      << " (SKU: " << node->product.sku << "): ";
            cin >> unitsSold;

            if (unitsSold > node->product.stock) {
                cout << "Error: Units sold cannot exceed available stock.\n";
            } else {
                node->product.unitsSold += unitsSold;
                updateStock(node, node->product.sku, -unitsSold);
            }

            recordSalesForProduct(node->right);
        }
    }
};

int main() {
    InventoryManagementSystem ims;
    cout << "            SMART INVENTORY SYSTEM              !\n";

    // Load products and transactions from files
    ims.loadProducts();
    ims.loadTransactions();

    ims.preAddProducts();

    while (true) {
        cout << "1. Add Product\n";
        cout << "2. Display Products\n";
        cout << "3. Update Stock\n";
        cout << "4. Recommend Stock Adjustments\n";
        cout << "5. Record Sales\n";
        cout << "6. View Transaction History\n";
        cout << "7. Save and Exit\n";
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
                ims.saveProducts();
                ims.saveTransactions();
                cout << "Exiting and saving data.\n";
                return 0;
            default:
                cout << "Invalid choice. Try again.\n";
        }
    }

    return 0;
}
