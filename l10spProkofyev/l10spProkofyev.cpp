#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>

using namespace std;

// Абстрактный класс Состояние
class CheeseState {
public:
    virtual ~CheeseState() {}
    virtual string getState() const = 0;
};

// Конкретные состояния сыра
class SolidCheeseState : public CheeseState {
public:
    string getState() const override {
        return "Solid";
    }
};

class MoldyCheeseState : public CheeseState {
public:
    string getState() const override {
        return "Moldy";
    }
};

// Контекст (Сыр)
class Cheese {
private:
    string brand;
    string type;
    float fat_content;
    float price;
    CheeseState* state;

public:
    Cheese(const string& brand, const string& type, float fat_content, float price)
        : brand(brand), type(type), fat_content(fat_content), price(price), state(nullptr) {}

    ~Cheese() {
        delete state;
    }

    void setState(CheeseState* newState) {
        delete state;
        state = newState;
    }

    string getState() const {
        if (state != nullptr) {
            return state->getState();
        }
        return "Unknown";
    }

    string getBrand() const {
        return brand;
    }

    string getType() const {
        return type;
    }

    float getFatContent() const {
        return fat_content;
    }

    float getPrice() const {
        return price;
    }
};

class CheeseProducer {
public:
    static CheeseProducer& getInstance() {
        static CheeseProducer instance;
        return instance;
    }

private:
    CheeseProducer() {}
    CheeseProducer(const CheeseProducer&) = delete;
    CheeseProducer& operator=(const CheeseProducer&) = delete;
};

class BinaryTree {
private:
    struct Node {
        Cheese* data;
        Node* left;
        Node* right;
        Node(Cheese* cheese) : data(cheese), left(nullptr), right(nullptr) {}
    };

    Node* root;
    static const int MAX_CHEESES = 100;
    Cheese* descriptions[MAX_CHEESES];
    int cheeseCount = 0;

    void addNode(Node*& node, Cheese* cheese) {
        if (node == nullptr) {
            node = new Node(cheese);
        }
        else {
            if (cheese->getBrand() + to_string(cheese->getPrice()) < node->data->getBrand() + to_string(node->data->getPrice())) {
                addNode(node->left, cheese);
            }
            else {
                addNode(node->right, cheese);
            }
        }
    }

    void deleteNode(Node*& node, const string& brand, float price) {
        if (node == nullptr) {
            throw invalid_argument("Cheese not found");
        }
        if (brand + to_string(price) < node->data->getBrand() + to_string(node->data->getPrice())) {
            deleteNode(node->left, brand, price);
        }
        else if (brand + to_string(price) > node->data->getBrand() + to_string(node->data->getPrice())) {
            deleteNode(node->right, brand, price);
        }
        else {
            if (node->left == nullptr && node->right == nullptr) {
                delete node->data;
                delete node;
                node = nullptr;
            }
            else if (node->left == nullptr) {
                Node* temp = node;
                node = node->right;
                delete temp->data;
                delete temp;
            }
            else if (node->right == nullptr) {
                Node* temp = node;
                node = node->left;
                delete temp->data;
                delete temp;
            }
            else {
                Node* temp = findMin(node->right);
                node->data = temp->data;
                deleteNode(node->right, temp->data->getBrand(), temp->data->getPrice());
            }
        }
    }

    Node* findMin(Node* node) {
        while (node->left != nullptr) {
            node = node->left;
        }
        return node;
    }

    bool searchNode(Node* node, const string& brand, float price) {
        if (node == nullptr) {
            return false;
        }
        if (brand + to_string(price) < node->data->getBrand() + to_string(node->data->getPrice())) {
            return searchNode(node->left, brand, price);
        }
        else if (brand + to_string(price) > node->data->getBrand() + to_string(node->data->getPrice())) {
            return searchNode(node->right, brand, price);
        }
        else {
            return true;
        }
    }

    void getDescriptions(Node* node) {
        if (node != nullptr) {
            getDescriptions(node->left);
            descriptions[cheeseCount++] = node->data;
            getDescriptions(node->right);
        }
    }

public:
    BinaryTree() : root(nullptr) {}

    void addCheese(Cheese* cheese) {
        addNode(root, cheese);
    }

    void removeCheese(const string& brand, float price) {
        try {
            deleteNode(root, brand, price);
        }
        catch (const invalid_argument& e) {
            cerr << "Error: " << e.what() << endl;
        }
    }

    bool searchCheese(const string& brand, float price) {
        return searchNode(root, brand, price);
    }

    void getAllDescriptions() {
        cheeseCount = 0;
        getDescriptions(root);
        cout << "Descriptions of all cheeses:\n";
        for (int i = 0; i < cheeseCount; ++i) {
            cout << "Brand: " << descriptions[i]->getBrand()
                << ", Type: " << descriptions[i]->getType()
                << ", Fat Content: " << descriptions[i]->getFatContent()
                << ", Price: " << descriptions[i]->getPrice()
                << ", State: " << descriptions[i]->getState() << endl;
        }
    }

    void saveToFile(const string& filename) {
        ofstream file(filename);
        if (!file.is_open()) {
            cerr << "Unable to open file " << filename << " for writing.\n";
            return;
        }

        for (int i = 0; i < cheeseCount; ++i) {
            file << descriptions[i]->getBrand() << " " << descriptions[i]->getType()
                << " " << descriptions[i]->getFatContent() << " " << descriptions[i]->getPrice() << endl;
        }

        file.close();
        cout << "Data saved to file " << filename << " successfully.\n";
    }

    void loadFromFile(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Unable to open file " << filename << " for reading.\n";
            return;
        }

        cheeseCount = 0;
        while (!file.eof()) {
            string brand, type;
            float fat_content, price;
            file >> brand >> type >> fat_content >> price;
            if (file.fail()) {
                break;
            }
            Cheese* cheese = new Cheese(brand, type, fat_content, price);
            cheese->setState(new SolidCheeseState()); // Устанавливаем начальное состояние "Твердый"
            addCheese(cheese);
        }

        file.close();
        cout << "Data loaded from file " << filename << " successfully.\n";
    }
};

int main() {
    setlocale(LC_ALL, "Russian");
    BinaryTree cheeseTree;

    while (true) {
        cout << "Выберите действие:\n"
            << "a. Добавить сыр\n"
            << "r. Удалить сыр\n"
            << "s. Проверить наличие сыра\n"
            << "d. Получить описания всех сыров\n"
            << "f. Сохранить данные в файл\n"
            << "l. Загрузить данные из файла\n"
            << "q. Выйти\n";
        char choice;
        cin >> choice;

        switch (choice) {
        case 'a': {
            string brand, type;
            float fat_content, price;
            cout << "Введите данные о сыре (бренд, тип, содержание жира, цена): ";
            cin >> brand >> type >> fat_content >> price;
            Cheese* newCheese = new Cheese(brand, type, fat_content, price);
            newCheese->setState(new SolidCheeseState()); // Устанавливаем начальное состояние "Твердый"
            cheeseTree.addCheese(newCheese);
            break;
        }
        case 'r': {
            string brand;
            float price;
            cout << "Введите бренд и цену сыра для удаления: ";
            cin >> brand >> price;
            cheeseTree.removeCheese(brand, price);
            break;
        }
        case 's': {
            string brand;
            float price;
            cout << "Введите бренд и цену сыра для проверки наличия: ";
            cin >> brand >> price;
            if (cheeseTree.searchCheese(brand, price)) {
                cout << "Сыр найден!\n";
            }
            else {
                cout << "Сыр не найден.\n";
            }
            break;
        }
        case 'd':
            cheeseTree.getAllDescriptions();
            break;
        case 'f': {
            string filename;
            cout << "Введите имя файла для сохранения данных: ";
            cin >> filename;
            cheeseTree.saveToFile(filename);
            break;
        }
        case 'l': {
            string filename;
            cout << "Введите имя файла для загрузки данных: ";
            cin >> filename;
            cheeseTree.loadFromFile(filename);
            break;
        }
        case 'q':
            cout << "Выход.\n";
            return 0;
        default:
            cout << "Некорректный ввод. Пожалуйста, выберите существующее действие.\n";
        }
    }

    return 0;
}
