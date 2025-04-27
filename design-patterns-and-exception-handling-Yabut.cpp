#include <iostream>     //input/output
#include <string>       //string handling
#include <iomanip>      //format
#include <cctype>       //isdigit, tolower
#include <fstream>      //log orders
using namespace std;

// Limits the maximum number of products, cart items, and orders
const int MAX_PRODUCTS = 100;
const int MAX_CART_ITEMS = 50;
const int MAX_ORDERS = 50;

// Abstract class for display
class Displayable {
public:
    virtual void display() const = 0;
};

// Payment Strategy (Strategy Pattern)
class PaymentStrategy {
public:
    virtual string pay() const = 0;
    virtual ~PaymentStrategy() {}
};

// Singleton Concrete Payment Strategies
class CashPayment : public PaymentStrategy {
private:
    CashPayment() {}
public:
    static CashPayment* getInstance() {
        static CashPayment instance;
        return &instance;
    }
    string pay() const override {
        return "Cash";
    }
};

class CreditCardPayment : public PaymentStrategy {
private:
    CreditCardPayment() {}
public:
    static CreditCardPayment* getInstance() {
        static CreditCardPayment instance;
        return &instance;
    }
    string pay() const override {
        return "Credit Card";
    }
};

class DebitCardPayment : public PaymentStrategy {
private:
    DebitCardPayment() {}
public:
    static DebitCardPayment* getInstance() {
        static DebitCardPayment instance;
        return &instance;
    }
    string pay() const override {
        return "Debit Card";
    }
};

class GcashPayment : public PaymentStrategy {
private:
    GcashPayment() {}
public:
    static GcashPayment* getInstance() {
        static GcashPayment instance;
        return &instance;
    }
    string pay() const override {
        return "GCash";
    }
};

// Product class
class Product : public Displayable {
public:
    string productName;
    int productID;
    double productPrice;

    Product() {}

    Product(int id, string name, double price) {
        productID = id;
        productName = name;
        productPrice = price;
    }

    void display() const override {
        cout << left << setw(12) << productID
             << setw(20) << productName
             << setw(12) << productPrice << endl;
    }
};

// Order class
class Order : public Displayable {
public:
    int orderID;
    string customerName;
    string customerContact;
    string customerAddress;
    double orderTotal;
    string orderPayment;
    Product orderItems[MAX_CART_ITEMS];
    int orderQuantities[MAX_CART_ITEMS];
    int itemCount;

    Order() {}

    Order(int id, string name, double total, Product items[], int quantities[], int count, string payment) {
        orderID = id;
        customerName = name;
        orderTotal = total;
        orderPayment = payment;
        itemCount = count;
        for (int i = 0; i < count; i++) {
            orderItems[i] = items[i];
            orderQuantities[i] = quantities[i];
        }
    }

    void display() const override {
        cout << "Order ID       : " << orderID << endl;
        cout << "Customer Name  : " << customerName << endl;
        cout << "Payment Method : " << orderPayment << endl;
        cout << "Total Amount   : " << orderTotal << endl;
        cout << "---------------------------------------------\n";
        cout << left << setw(12) << "Product ID" << setw(20) << "Name"
             << setw(10) << "Price" << setw(10) << "Quantity" << endl;
        cout << "---------------------------------------------\n";
        for (int i = 0; i < itemCount; i++) {
            cout << left << setw(12) << orderItems[i].productID
                 << setw(20) << orderItems[i].productName
                 << setw(10) << orderItems[i].productPrice
                 << setw(10) << orderQuantities[i] << endl;
        }
        cout << "=======================================\n\n";
    }
};

// ShoppingCart class
class ShoppingCart {
private:
    Product* productList[MAX_CART_ITEMS];
    int productQty[MAX_CART_ITEMS];
    int itemCount = 0;
    double totalAmount = 0;

    Order orderHistory[MAX_ORDERS];
    int orderCounter = 1;
    int orderCount = 0;

    bool isValidContact(string contact) {
        if (contact.length() != 11) return false;
        for (char c : contact) {
            if (!isdigit(c)) return false;
        }
        return true;
    }

public:
    void addProduct(Product* product, int quantity) {
        // Check if the product already exists in cart
        bool found = false;
        for (int i = 0; i < itemCount; i++) {
            if (productList[i]->productID == product->productID) {
                // found, increase quantity and update total
                productQty[i] += quantity;
                totalAmount += product->productPrice * quantity;
                found = true;
                break;
            }
        }

        if (!found) {
            // not found add new entry
            productList[itemCount] = product;
            productQty[itemCount] = quantity;
            totalAmount += product->productPrice * quantity;
            itemCount++;
        }

        cout << "Product added successfully!\n";
    }

    void checkoutOrder() {
        if (itemCount == 0) {
            cout << "Your cart is empty. Add products before checkout.\n";
            return;
        }

        string name;
        cin.ignore();
        cout << "\n=======================================\n";
        cout << "            CHECKOUT FORM\n";
        cout << "=======================================\n";
        cout << "Enter customer name  : ";
        getline(cin, name);

        PaymentStrategy* paymentMethod = nullptr;
        string paymentType;
        do {
            cout << "Enter payment method (Cash, Credit Card, Debit Card, GCash): ";
            getline(cin, paymentType);
            for (char& c : paymentType) c = tolower(c);

            if (paymentType == "cash") paymentMethod = CashPayment::getInstance();
            else if (paymentType == "credit card") paymentMethod = CreditCardPayment::getInstance();
            else if (paymentType == "debit card") paymentMethod = DebitCardPayment::getInstance();
            else if (paymentType == "gcash") paymentMethod = GcashPayment::getInstance();
            else cout << "Invalid payment method!\n";

        } while (paymentMethod == nullptr);

        string paymentUsed = paymentMethod->pay();

        Product items[MAX_CART_ITEMS];
        int quantities[MAX_CART_ITEMS];
        for (int i = 0; i < itemCount; i++) {
            items[i] = *productList[i];
            quantities[i] = productQty[i];
        }

        // Updated constructor call without the address parameter
        orderHistory[orderCount++] = Order(orderCounter, name, totalAmount, items, quantities, itemCount, paymentUsed);

        // LOGGING TO FILE
        ofstream logFile("order_log.txt", ios::app); 
        if (logFile.is_open()) {
            logFile << "Order ID: " << orderCounter
                    << " has been successfully checked out and paid using "
                    << paymentUsed << "." << endl;
            logFile.close();
        } else {
            cerr << "Error opening log file!" << endl;
        }

        cout << "Order ID: " << orderCounter
             << " has been successfully checked out and paid using "
             << paymentUsed << "." << endl;

        cout << "You have successfully checked out the products!\n";

        orderCounter++;
        clearCart();
    }

    void viewCart() {
        if (itemCount == 0) {
            cout << "Your cart is empty!\n";
            return;
        }
        cout << "\n=======================================\n";
        cout << "             SHOPPING CART\n";
        cout << "=======================================\n";
        cout << left << setw(10) << "ID" << setw(20) << "Name"
             << setw(10) << "Price" << setw(10) << "Qty" << endl;
        cout << "---------------------------------------\n";
        for (int i = 0; i < itemCount; i++) {
            cout << left << setw(10) << productList[i]->productID
                 << setw(20) << productList[i]->productName
                 << setw(10) << productList[i]->productPrice
                 << setw(10) << productQty[i] << endl;
        }
        cout << "---------------------------------------\n";
        cout << "Total: " << totalAmount << endl;
        cout << "=======================================\n";

        char checkoutChoice;
        cout << "Do you want to proceed to checkout? (Y/N): ";
        cin >> checkoutChoice;
        checkoutChoice = tolower(checkoutChoice);
        if (checkoutChoice == 'y') {
            checkoutOrder();
        }
    }

    void viewOrders() {
        if (orderCount == 0) {
            cout << "No orders placed yet!\n";
            return;
        }

        cout << "\n=======================================\n";
        cout << "            ORDER DETAILS\n";
        cout << "=======================================\n";

        for (int i = 0; i < orderCount; i++) {
            orderHistory[i].display();
        }
    }

    void clearCart() {
        itemCount = 0;
        totalAmount = 0;
    }
};

int main() {
    Product products[] = {
        {101, "Notebook", 100.0},
        {102, "Pen", 20.0},
        {103, "Pencil", 15.0},
        {104, "Eraser", 10.0},
        {105, "Clip", 10.0}
    };

    ShoppingCart cart;
    bool running = true;
    int choice;

    while (running) {
        cout << "\n[1] View Products\n[2] View Shopping Cart\n[3] View Orders\n[4] Exit\n";

        string input;
        cout << "Enter your choice: ";
        cin >> input;

        try {
            size_t pos;
            choice = stoi(input, &pos);
            if (pos != input.length() || choice < 1 || choice > 4) {
                throw invalid_argument("Invalid");
            }
        } catch (...) {
            cout << "Invalid choice! Please enter 1, 2, 3, or 4 only.\n";
            continue;
        }

        switch (choice) {
            case 1: {
                cout << "\n========================================================\n";
                cout << "                 AVAILABLE PRODUCTS\n";
                cout << "========================================================\n";
                cout << left << setw(12) << "ID" << setw(20) << "Name"
                     << setw(12) << "Price\n";
                cout << "----------------------------------------------------------\n";
                for (Product& p : products) {
                    p.display();
                }
                cout << "----------------------------------------------------------\n";
            
                char addMore = 'y';
                while (addMore == 'y') {
                    int id;
                    bool validID = false;
            
                    while (!validID) {
                        cout << "Enter product ID to add to cart: ";
                        string inputID;
                        cin >> inputID;
            
                        try {
                            size_t pos;
                            id = stoi(inputID, &pos);
                            if (pos != inputID.length()) throw invalid_argument("Non-numeric");
            
                            // Check if ID exists in product list
                            for (Product& p : products) {
                                if (p.productID == id) {
                                    validID = true;
                                    break;
                                }
                            }
                            if (!validID) {
                                cout << "Invalid product ID! Try again.\n";
                            }
                        } catch (...) {
                            cout << "Invalid input! Enter numeric product ID only.\n";
                        }
                    }
            
                    int quantity;
                    bool validQty = false;
                    while (!validQty) {
                        cout << "Enter quantity: ";
                        string inputQty;
                        cin >> inputQty;
            
                        try {
                            size_t pos;
                            quantity = stoi(inputQty, &pos);
                            if (pos != inputQty.length() || quantity <= 0) {
                                throw invalid_argument("Invalid input");
                            }
                            validQty = true;
                        } catch (invalid_argument&) {
                            cout << "Invalid quantity! Enter positive whole numbers only.\n";
                        } catch (out_of_range&) {
                            cout << "Number is too large! Try a smaller value.\n";
                        }
                    }
            
                    for (Product& p : products) {
                        if (p.productID == id) {
                            cart.addProduct(&p, quantity);
                            break;
                        }
                    }
            
                    bool validYN = false;
                    while (!validYN) {
                        string input;
                        cout << "Add another product? (Y/N): ";
                        cin >> input;
                        if (input.length() == 1) {
                            addMore = tolower(input[0]);
                            if (addMore == 'y' || addMore == 'n') {
                                validYN = true;
                            } else {
                                cout << "Enter only Y or N.\n";
                            }
                        } else {
                            cout << "Enter only Y or N.\n";
                        }
                    }
                }
                break;
            }
            
            case 2:
                cart.viewCart();
                break;
            case 3:
                cart.viewOrders();
                break;
            case 4:
                running = false;
                break;
        }
    }

    return 0;
}