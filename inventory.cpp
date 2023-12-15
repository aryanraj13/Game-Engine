#include <iostream>
#include <string>
using namespace std;

class GameItem {
public:
    string name;
    int quantity;

    GameItem(string n, int q) : name(n), quantity(q) {}

    void displayItem() {
        cout << name << " (" << quantity << ")" << endl;
    }
};

class AVLNode {
public:
    GameItem* item;
    AVLNode* left;
    AVLNode* right;
    int height;

    AVLNode(GameItem* item) : item(item), left(nullptr), right(nullptr), height(1) {}
};

class AVLTree {
public:
    AVLNode* root;

    AVLTree() : root(nullptr) {}

    int getHeight(AVLNode* node) {
        if (node == nullptr)
            return 0;
        return node->height;
    }

    int getBalance(AVLNode* node) {
        if (node == nullptr)
            return 0;
        return getHeight(node->left) - getHeight(node->right);
    }

    AVLNode* rightRotate(AVLNode* y) {
        AVLNode* x = y->left;
        AVLNode* T2 = x->right;

        x->right = y;
        y->left = T2;

        y->height = 1 + max(getHeight(y->left), getHeight(y->right));
        x->height = 1 + max(getHeight(x->left), getHeight(x->right));

        return x;
    }

    AVLNode* leftRotate(AVLNode* x) {
        AVLNode* y = x->right;
        AVLNode* T2 = y->left;

        y->left = x;
        x->right = T2;

        x->height = 1 + max(getHeight(x->left), getHeight(x->right));
        y->height = 1 + max(getHeight(y->left), getHeight(y->right));

        return y;
    }

    AVLNode* insert(AVLNode* node, GameItem* item) {
        if (node == nullptr)
            return new AVLNode(item);

        if (item->name < node->item->name)
            node->left = insert(node->left, item);
        else if (item->name > node->item->name)
            node->right = insert(node->right, item);
        else {
            node->item->quantity += item->quantity;
            return node;
        }

        node->height = 1 + max(getHeight(node->left), getHeight(node->right));

        int balance = getBalance(node);

        if (balance > 1) {
            if (item->name < node->left->item->name)
                return rightRotate(node);
            else {
                node->left = leftRotate(node->left);
                return rightRotate(node);
            }
        }

        if (balance < -1) {
            if (item->name > node->right->item->name)
                return leftRotate(node);
            else {
                node->right = rightRotate(node->right);
                return leftRotate(node);
            }
        }

        return node;
    }

    void addItem(string name, int quantity) {
        GameItem* item = new GameItem(name, quantity);
        root = insert(root, item);
        cout << "Added: " << name << " (" << quantity << ")" << endl;
    }

    AVLNode* findMinNode(AVLNode* node) {
        AVLNode* current = node;
        while (current->left != nullptr)
            current = current->left;
        return current;
    }

    AVLNode* remove(AVLNode* node, GameItem* item) {
        if (node == nullptr)
            return node;

        if (item->name < node->item->name)
            node->left = remove(node->left, item);
        else if (item->name > node->item->name)
            node->right = remove(node->right, item);
        else {
            if (item->quantity < node->item->quantity) {
                node->item->quantity -= item->quantity;
                return node;
            }
            if (node->left == nullptr || node->right == nullptr) {
                AVLNode* temp = node->left ? node->left : node->right;

                if (temp == nullptr) {
                    temp = node;
                    node = nullptr;
                } else {
                    *node = *temp;
                }

                delete temp;
            } else {
                AVLNode* temp = findMinNode(node->right);
                node->item = temp->item;
                node->right = remove(node->right, temp->item);
            }
        }

        if (node == nullptr)
            return node;

        node->height = 1 + max(getHeight(node->left), getHeight(node->right));

        int balance = getBalance(node);

        if (balance > 1) {
            if (getBalance(node->left) >= 0)
                return rightRotate(node);
            else {
                node->left = leftRotate(node->left);
                return rightRotate(node);
            }
        }

        if (balance < -1) {
            if (getBalance(node->right) <= 0)
                return leftRotate(node);
            else {
                node->right = rightRotate(node->right);
                return leftRotate(node);
            }
        }

        return node;
    }

    AVLNode* search(AVLNode* node, string name) {
        if (node == nullptr || node->item->name == name)
            return node;

        if (name < node->item->name)
            return search(node->left, name);
        else
            return search(node->right, name);
    }

    void displayInOrder(AVLNode* node) {
        if (node) {
            displayInOrder(node->left);
            node->item->displayItem();
            displayInOrder(node->right);
        }
    }

    int getTotalQuantity(AVLNode* node, string name) {
        if (node == nullptr)
            return 0;

        if (name < node->item->name)
            return getTotalQuantity(node->left, name);
        else if (name > node->item->name)
            return getTotalQuantity(node->right, name);
        else
            return node->item->quantity;
    }

    void displayInventory() {
        if (root == nullptr) {
            cout << "Inventory is empty." << endl;
        } else {
            cout << "Inventory:" << endl;
            displayInOrder(root);
        }
    }

    void removeItem(string name, int quantity) {
        GameItem* item = new GameItem(name, quantity);
        root = remove(root, item);
        cout << "Removed: " << name << " (" << quantity << ")" << endl;
    }

    void searchItem(string name) {
        AVLNode* result = search(root, name);
        if (result)
            cout << "Found: " << result->item->name << " (" << result->item->quantity << ")" << endl;
        else
            cout << "Item '" << name << "' not found." << endl;
    }

    void displayTotalQuantity(string name) {
        int totalQuantity = getTotalQuantity(root, name);
        if (totalQuantity > 0)
            cout << "Total quantity of " << name << ": " << totalQuantity << endl;
        else
            cout << "Item '" << name << "' not found in inventory." << endl;
    }
};

int main() {
    AVLTree inventory;

    
    inventory.addItem("Sword", 3);
    inventory.addItem("Potion", 5);
    inventory.addItem("Shield", 2);
    inventory.addItem("Potion", 3); 

    inventory.displayInventory();

    inventory.removeItem("Potion", 1);

    inventory.searchItem("Potion");

    inventory.displayTotalQuantity("Potion");
    inventory.displayTotalQuantity("Sword");

    return 0;
}