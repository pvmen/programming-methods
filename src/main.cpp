#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <chrono>
#include <cstdlib>
#include <map>

/**
 * @brief Запись об экспортируемом товаре.
 */
struct Product {
  std::string name;     ///< Наименование товара.
  std::string country;  ///< Страна экспорта.
  int volume;           ///< Объем поставляемой продукции.
  double rubles;        ///< Сумма поставки в рублях.
};

/**
 * @brief Элемент хеш-таблицы для одного ключа.
 */
struct HashEntry{
  std::string key;                 ///< Ключ поиска.
  std::vector<Product> products;   ///< Все товары с данным ключом.
};

/**
 * @brief Узел бинарного дерева поиска.
 */
struct TreeNode{
  std::string key;               ///< Ключ узла.
  std::vector<Product> products; ///< Все товары с данным ключом.
  TreeNode* left;                ///< Левый потомок.
  TreeNode* right;               ///< Правый потомок.
};

/**
 * @brief Узел красно-черного дерева.
 */
struct RBNode {
  std::string key;               ///< Ключ узла.
  std::vector<Product> products; ///< Все товары с данным ключом.

  RBNode* left;                  ///< Левый потомок.
  RBNode* right;                 ///< Правый потомок.
  RBNode* parent;                ///< Родительский узел.

  bool red;                      ///< Цвет узла: true - красный, false - черный.
};

/**
 * @brief Создает узел бинарного дерева поиска.
 * @param product Товар, который будет сохранен в новом узле.
 * @return Указатель на созданный узел.
 */
TreeNode* createTreeNode(Product& product){
  TreeNode* node = new TreeNode();

  node->key = product.name;
  node->products.push_back(product);
  node->left = nullptr;
  node->right = nullptr;

  return node;
}

/**
 * @brief Вставляет товар в бинарное дерево поиска.
 * @param root Корень дерева.
 * @param product Вставляемый товар.
 * @return Корень дерева после вставки.
 */
TreeNode* insertTreeNode(TreeNode* root, Product& product){
  if (root == nullptr){
    return createTreeNode(product);
  }
  if (product.name == root->key){
    root->products.push_back(product);
  } else if (product.name < root->key){
    root->left = insertTreeNode(root->left, product);
  } else if (product.name > root->key){
    root->right = insertTreeNode(root->right, product);
  }

  return root;
}

/**
 * @brief Строит бинарное дерево поиска по массиву товаров.
 * @param products Массив товаров.
 * @return Корень построенного дерева.
 */
TreeNode* buildTree(std::vector<Product>& products){
  TreeNode* root = nullptr;
  for (std::size_t i = 0; i < products.size(); i++){
    root = insertTreeNode(root, products[i]);
  }
  return root;
}

/**
 * @brief Ищет все товары с заданным ключом в бинарном дереве поиска.
 * @param root Корень дерева.
 * @param key Искомое наименование товара.
 * @return Массив найденных товаров.
 */
std::vector<Product> searchTree(TreeNode* root, std::string key){
  if (root == nullptr){
    return std::vector<Product>();
  }
  if (key == root->key){
    return root->products;
  }
  if (key < root->key){
    return searchTree(root->left, key);
  }
  
  return searchTree(root->right, key);
  
}
/**
 * @brief Выводит один товар в консоль.
 * @param product Товар для вывода.
 */
void printProduct(Product& product) {
  std::cout << product.name << " " << product.country << " " << product.volume << " " << product.rubles << "\n";
}

/**
 * @brief Выполняет линейный поиск всех товаров по названию.
 * @param products Массив товаров.
 * @param key Искомое наименование товара.
 * @return Массив найденных товаров.
 */
std::vector<Product> linearSearch(std::vector<Product>& products, std::string key){
  std::vector<Product> result;
  for (std::size_t i = 0; i < products.size(); i++){
    if (products[i].name == key){
      result.push_back(products[i]);
    }
  }
  return result;
}

/**
 * @brief Записывает товары в текстовый файл.
 * @param filename Имя файла для записи.
 * @param products Массив товаров.
 */
void writeProductsToFile(std::string filename, std::vector<Product>& products){
  std::ofstream file(filename);
  for (std::size_t i = 0; i < products.size(); i++){
    file << products[i].name << " " << products[i].country << " " << products[i].volume << " " << products[i].rubles << "\n";
  }
  file.close();
}

/**
 * @brief Генерирует случайные товары и записывает их в файл.
 * @param filename Имя файла для записи.
 * @param count Количество генерируемых записей.
 */
void generateProductsToFile(std::string filename, int count){
  std::ofstream file(filename);
  std::vector<std::string> names = {"Coal", "Gas", "Oil", "Wood"};
  std::vector<std::string> countries = {"China", "Turkey", "Armenia", "Belarus"};
  for (int i = 0; i < count; i++){
    std::string name = names[std::rand() % names.size()];
    std::string country = countries[std::rand() % countries.size()];
    int volume = std::rand() % 1000 + 100;
    double rubles = std::rand() % 516600 + 1000;
    file << name << " "
    << country << " "
    << volume << " "
    << rubles << "\n";
  }
}

/**
 * @brief Читает товары из текстового файла.
 * @param filename Имя файла для чтения.
 * @return Массив товаров из файла.
 */
std::vector<Product> readProductsFromFile(std::string filename){
  std::vector<Product> products;
  std::ifstream file(filename);

  Product product;
  while(file >> product.name >> product.country >> product.volume >> product.rubles){
    products.push_back(product);
  }
  return products;
}

/**
 * @brief Выводит список товаров в консоль.
 * @param products Массив товаров для вывода.
 */
void printProducts(std::vector<Product>& products) {
  for (std::size_t i = 0; i < products.size(); i++){
    printProduct(products[i]);
  }
}

/**
 * @brief Вычисляет хеш для строкового ключа.
 * @param key Строковый ключ.
 * @param tableSize Размер хеш-таблицы.
 * @return Индекс корзины в хеш-таблице.
 */
std::size_t hashFunction(std::string key, std::size_t tableSize){
  std::size_t hash = 0;

  for (std::size_t i = 0; i < key.size(); i++){
    hash = hash + key[i];
  }

  return hash % tableSize;
}


/**
 * @brief Вставляет товар в хеш-таблицу с разрешением коллизий цепочками.
 * @param table Хеш-таблица.
 * @param product Вставляемый товар.
 * @return true, если вставка попала в уже занятую корзину.
 */
bool insertHashTable(std::vector<std::vector<HashEntry>>& table, Product& product) {
  std::size_t index = hashFunction(product.name, table.size());
  bool hasCollision = table[index].size() > 0;

  for (std::size_t i = 0; i < table[index].size(); i++) {
    if (table[index][i].key == product.name) {
      table[index][i].products.push_back(product);
      return hasCollision;
    }
  }

  HashEntry entry;
  entry.key = product.name;
  entry.products.push_back(product);

  table[index].push_back(entry);

  return hasCollision;
}


/**
 * @brief Строит хеш-таблицу по массиву товаров.
 * @param products Массив товаров.
 * @param tableSize Размер хеш-таблицы.
 * @param collisions Счетчик коллизий хеш-функции.
 * @return Построенная хеш-таблица.
 */
std::vector<std::vector<HashEntry>> buildHashTable(std::vector<Product>& products, std::size_t tableSize, int& collisions){
  std::vector<std::vector<HashEntry>> table(tableSize);
  collisions = 0;

  for (std::size_t i = 0; i < products.size(); i++){
    bool hasCollision = insertHashTable(table, products[i]);

    if (hasCollision){
      collisions++;
    }
  }

  return table;
}

/**
 * @brief Ищет все товары с заданным ключом в хеш-таблице.
 * @param table Хеш-таблица.
 * @param key Искомое наименование товара.
 * @return Массив найденных товаров.
 */
std::vector<Product> searchHashTable(std::vector<std::vector<HashEntry>>& table, std::string key){
  std::size_t index = hashFunction(key, table.size());

  for (std::size_t i = 0; i < table[index].size(); i++){
    if (table[index][i].key == key){
      return table[index][i].products;
    }
  }

  return std::vector<Product>();
}

/**
 * @brief Строит ассоциативный массив multimap по массиву товаров.
 * @param products Массив товаров.
 * @return Ассоциативный массив, где ключом является наименование товара.
 */
std::multimap<std::string, Product> buildMap(std::vector<Product>& products){
  std::multimap<std::string, Product> map;

  for (std::size_t i = 0; i < products.size(); i++){
    map.insert(std::make_pair(products[i].name, products[i]));
  }

  return map;
}

/**
 * @brief Ищет все товары с заданным ключом в multimap.
 * @param map Ассоциативный массив товаров.
 * @param key Искомое наименование товара.
 * @return Массив найденных товаров.
 */
std::vector<Product> searchMap(std::multimap<std::string, Product>& map, std::string key){
  std::vector<Product> result;

  auto range = map.equal_range(key);

  for (auto it = range.first; it != range.second; it++){
    result.push_back(it->second);
  }

  return result;

}

/**
 * @brief Создает узел красно-черного дерева.
 * @param product Товар, который будет сохранен в новом узле.
 * @return Указатель на созданный узел.
 */
RBNode* createRBNode(Product& product) {
  RBNode* node = new RBNode();

  node->key = product.name;
  node->products.push_back(product);

  node->left = nullptr;
  node->right = nullptr;
  node->parent = nullptr;

  node->red = true;

  return node;
}

/**
 * @brief Рекурсивно вставляет товар в обычное бинарное дерево на узлах RBNode.
 * @param root Корень дерева.
 * @param product Вставляемый товар.
 * @return Корень дерева после вставки.
 */
RBNode* insertRBNode(RBNode* root, Product& product) {
  if (root == nullptr) {
    return createRBNode(product);
  }

  if (product.name == root->key) {
    root->products.push_back(product);
  } else if (product.name < root->key) {
    root->left = insertRBNode(root->left, product);
    root->left->parent = root;
  } else {
    root->right = insertRBNode(root->right, product);
    root->right->parent = root;
  }

  return root;
}

void insertRBTree(RBNode*& root, Product& product);

/**
 * @brief Строит красно-черное дерево по массиву товаров.
 * @param products Массив товаров.
 * @return Корень построенного красно-черного дерева.
 */
RBNode* buildRBTree(std::vector<Product>& products) {
  RBNode* root = nullptr;

  for (std::size_t i = 0; i < products.size(); i++) {
    insertRBTree(root, products[i]);
  }

  return root;
}

/**
 * @brief Ищет все товары с заданным ключом в красно-черном дереве.
 * @param root Корень красно-черного дерева.
 * @param key Искомое наименование товара.
 * @return Массив найденных товаров.
 */
std::vector<Product> searchRBTree(RBNode* root, std::string key) {
  if (root == nullptr) {
    return std::vector<Product>();
  }

  if (key == root->key) {
    return root->products;
  }

  if (key < root->key) {
    return searchRBTree(root->left, key);
  }

  return searchRBTree(root->right, key);
}

/**
 * @brief Выполняет левый поворот в красно-черном дереве.
 * @param root Корень дерева.
 * @param x Узел, вокруг которого выполняется поворот.
 */
void rotateLeft(RBNode*& root, RBNode* x) {
  RBNode* y = x->right;

  x->right = y->left;

  if (y->left != nullptr) {
    y->left->parent = x;
  }

  y->parent = x->parent;

  if (x->parent == nullptr) {
    root = y;
  } else if (x == x->parent->left) {
    x->parent->left = y;
  } else {
    x->parent->right = y;
  }

  y->left = x;
  x->parent = y;
}

/**
 * @brief Выполняет правый поворот в красно-черном дереве.
 * @param root Корень дерева.
 * @param x Узел, вокруг которого выполняется поворот.
 */
void rotateRight(RBNode*& root, RBNode* x) {
  RBNode* y = x->left;

  x->left = y->right;

  if (y->right != nullptr) {
    y->right->parent = x;
  }

  y->parent = x->parent;

  if (x->parent == nullptr) {
    root = y;
  } else if (x == x->parent->right) {
    x->parent->right = y;
  } else {
    x->parent->left = y;
  }

  y->right = x;
  x->parent = y;
}

/**
 * @brief Восстанавливает свойства красно-черного дерева после вставки.
 * @param root Корень дерева.
 * @param node Вставленный узел.
 */
void fixRBInsert(RBNode*& root, RBNode* node) {
  while (node != root && node->parent->red) {
    RBNode* parent = node->parent;
    RBNode* grandparent = parent->parent;

    if (parent == grandparent->left) {
      RBNode* uncle = grandparent->right;

      if (uncle != nullptr && uncle->red) {
        parent->red = false;
        uncle->red = false;
        grandparent->red = true;
        node = grandparent;
      } else {
        if (node == parent->right) {
          node = parent;
          rotateLeft(root, node);
          parent = node->parent;
          grandparent = parent->parent;
        }

        parent->red = false;
        grandparent->red = true;
        rotateRight(root, grandparent);
      }
    } else {
      RBNode* uncle = grandparent->left;

      if (uncle != nullptr && uncle->red) {
        parent->red = false;
        uncle->red = false;
        grandparent->red = true;
        node = grandparent;
      } else {
        if (node == parent->left) {
          node = parent;
          rotateRight(root, node);
          parent = node->parent;
          grandparent = parent->parent;
        }

        parent->red = false;
        grandparent->red = true;
        rotateLeft(root, grandparent);
      }
    }
  }

  root->red = false;
}

/**
 * @brief Вставляет товар в красно-черное дерево.
 * @param root Корень дерева.
 * @param product Вставляемый товар.
 */
void insertRBTree(RBNode*& root, Product& product) {
  RBNode* parent = nullptr;
  RBNode* current = root;

  while (current != nullptr) {
    parent = current;

    if (product.name == current->key) {
      current->products.push_back(product);
      return;
    }

    if (product.name < current->key) {
      current = current->left;
    } else {
      current = current->right;
    }
  }

  RBNode* newNode = createRBNode(product);
  newNode->parent = parent;

  if (parent == nullptr) {
    root = newNode;
  } else if (product.name < parent->key) {
    parent->left = newNode;
  } else {
    parent->right = newNode;
  }

  fixRBInsert(root, newNode);
}

/**
 * @brief Точка входа программы.
 *
 * Генерирует входные данные разных размерностей, выполняет поиск
 * заданного ключа всеми методами, записывает времена поиска и число
 * коллизий хеш-функции в файл data/search_times.txt.
 *
 * @return Код завершения программы.
 */
int main() {

  std::vector<int> sizes = {
    100, 500, 1000, 5000, 10000,
    50000, 100000, 250000, 500000, 1000000
  };
   
   std::srand(1);

   std::ofstream timeFile("data/search_times.txt");
   timeFile << "size linear tree hash multimap red_black collisions\n";

   for (std::size_t i = 0; i < sizes.size(); i++) {
    int size = sizes[i];
    
    std::string inputFileName = "data/search_input" + std::to_string(size) + ".txt";
    
    generateProductsToFile(inputFileName, size);
    std::vector<Product> products = readProductsFromFile(inputFileName);

    std::string key = "Oil";

    auto linearStart = std::chrono::high_resolution_clock::now();
    std::vector<Product> linearFoundProducts = linearSearch(products, key);
    auto linearEnd = std::chrono::high_resolution_clock::now();

    auto linearTime = std::chrono::duration_cast<std::chrono::nanoseconds>(linearEnd - linearStart).count();

    TreeNode* root = buildTree(products);

    auto treeStart = std::chrono::high_resolution_clock::now();
    std::vector<Product> treeFoundProducts = searchTree(root, key);
    auto treeEnd = std::chrono::high_resolution_clock::now();

    auto treeTime = std::chrono::duration_cast<std::chrono::nanoseconds>(treeEnd - treeStart).count();

    int collisions = 0;
    std::vector<std::vector<HashEntry>> hashTable = buildHashTable(products, 10, collisions);

    auto hashStart = std::chrono::high_resolution_clock::now();
    std::vector<Product> hashFoundProducts = searchHashTable(hashTable, key);
    auto hashEnd = std::chrono::high_resolution_clock::now();

    auto hashTime = std::chrono::duration_cast<std::chrono::nanoseconds>(hashEnd - hashStart).count();

    std::multimap<std::string, Product> productMap = buildMap(products);

    auto mapStart = std::chrono::high_resolution_clock::now();
    std::vector<Product> mapFoundProducts = searchMap(productMap, key);
    auto mapEnd = std::chrono::high_resolution_clock::now();

    auto mapTime = std::chrono::duration_cast<std::chrono::nanoseconds>(mapEnd - mapStart).count();

    RBNode* rbRoot = buildRBTree(products);

    auto rbStart = std::chrono::high_resolution_clock::now();
    std::vector<Product> rbFoundProducts = searchRBTree(rbRoot, key);
    auto rbEnd = std::chrono::high_resolution_clock::now();

    auto rbTime = std::chrono::duration_cast<std::chrono::nanoseconds>(rbEnd - rbStart).count();

    timeFile << size << " "
         << linearTime << " "
         << treeTime << " "
         << hashTime << " "
         << mapTime << " "
         << rbTime << " "
         << collisions << "\n";

    std::cout << "size: " << size << "\n";
    std::cout << "linear: " << linearFoundProducts.size() << "\n";
    std::cout << "linear time: " << linearTime << " nanoseconds\n";
    std::cout << "binary tree: " << treeFoundProducts.size() << "\n";
    std::cout << "binary tree time: " << treeTime << " nanoseconds\n";
    std::cout << "hash table: " << hashFoundProducts.size() << "\n";
    std::cout << "hash table time: " << hashTime << " nanoseconds\n";
    std::cout << "hash collisions: " << collisions << "\n";
    std::cout << "multimap: " << mapFoundProducts.size() << "\n";
    std::cout << "multimap time: " << mapTime << " nanoseconds\n";
    std::cout << "red-black tree: " << rbFoundProducts.size() << "\n";
    std::cout << "red-black tree time: " << rbTime << " nanoseconds\n";
    std::cout << "\n";  
  }

   // // generateProductsToFile("data/input_laba2.txt", 100);
   // std::vector<Product> products = readProductsFromFile("data/input_laba2.txt");
// 
   // std::vector<Product> linearFoundProducts = linearSearch(products, "Oil");
   // // std::cout << "линейный поиск count of found products is: " << linearFoundProducts.size() << "\n";
   // // printProducts(linearFoundProducts);
// 
   // TreeNode* root = buildTree(products);
   // std::vector<Product> treeFoundProducts = searchTree(root, "Oil");
   // std::cout << "бинарное дерево count of Oil is  " << treeFoundProducts.size() << "\n";
   // // printProducts(treeFoundProducts);
// 
   // // std::vector<int> sizes = {2000, 5000, 10000, 20000, 101000};
   // // std::vector<int> sizes = {100, 500, 1000, 2000, 1010};
   // int collisions = 0;
   // std::vector<std::vector<HashEntry>> hashTable = buildHashTable(products, 10, collisions);
// 
   // std::vector<Product> hashFoundProducts = searchHashTable(hashTable, "Oil");
   // std::cout << "хеш-таблица count of Oil is  " << hashFoundProducts.size() << "\n";
   // // printProducts(hashFoundProducts);
// 
   // std::cout << "hash collisions: " << collisions << "\n"; 
// 
// 
   // std::multimap<std::string, Product> productMap = buildMap(products);
   // std::vector<Product> mapFoundProducts = searchMap(productMap, "Oil");
   // std::cout << "multimap count of Oil is " << mapFoundProducts.size() << "\n";
// 
   // RBNode* rbRoot = buildRBTree(products);
   // std::vector<Product> rbFoundProducts = searchRBTree(rbRoot, "Oil");
   // std::cout << "красно-черное дерево count of Oil is " << rbFoundProducts.size() << "\n";

   return 0;
}