#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <chrono>
#include <cstdlib>

/**
 * Запись об экспортируемом товаре.
 */
struct Product {
  std::string name;
  std::string country; // куда экспорт
  int volume; // объем поставляемой продукции 
  double rubles;
};

struct TreeNode{
  std::string key; // название товара
  std::vector<Product> products;
  TreeNode* left;
  TreeNode* right;
};
// короче у узла node есть поле products, это vector<Product>
// в него добавляем product
TreeNode* createTreeNode(Product& product){
  TreeNode* node = new TreeNode();

  node->key = product.name;
  node->products.push_back(product); // внутрь вектора пушбекаем
  node->left = nullptr;
  node->right = nullptr;

  return node;
}

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

TreeNode* buildTree(std::vector<Product>& products){
  TreeNode* root = nullptr;
  for (std::size_t i = 0; i < products.size(); i++){
    root = insertTreeNode(root, products[i]);
  }
  return root;
}

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
 * Выводит один товар в консоль.
 */
void printProduct(Product& product) {
  std::cout << product.name << " " << product.country << " " << product.volume << " " << product.rubles << "\n";
}

/**
 * Выполняет линейный поиск всех товаров по названию.
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
 * Записывает товары в текстовый файл.
 * Каждый товар записывается на отдельной строке.
 */
void writeProductsToFile(std::string filename, std::vector<Product>& products){
  std::ofstream file(filename);
  for (std::size_t i = 0; i < products.size(); i++){
    file << products[i].name << " " << products[i].country << " " << products[i].volume << " " << products[i].rubles << "\n";
  }
  file.close();
}

/**
 * Генерирует случайные товары и записывает их в файл.
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
 * Читает товары из текстового файла.
 * В каждой строке должны быть name, country, volume и rubles.
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
 * Выводит список товаров в консоль.
 */
void printProducts(std::vector<Product>& products) {
  for (std::size_t i = 0; i < products.size(); i++){
    printProduct(products[i]);
  }
}

/**
 * Точка входа программы.
 */
int main() {
   
   std::srand(1);

   generateProductsToFile("data/input_laba2.txt", 100);
   std::vector<Product> products = readProductsFromFile("data/input_laba2.txt");

   std::vector<Product> linearFoundProducts = linearSearch(products, "Oil");
   std::cout << "линейный поиск count of found products is: " << linearFoundProducts.size() << "\n";
   printProducts(linearFoundProducts);

   TreeNode* root = buildTree(products);
   std::vector<Product> treeFoundProducts = searchTree(root, "Oil");
   std::cout << "бинарное дерево count of Oil is  " << treeFoundProducts.size() << "\n";
   printProducts(treeFoundProducts);

   // std::vector<int> sizes = {2000, 5000, 10000, 20000, 101000};
   // std::vector<int> sizes = {100, 500, 1000, 2000, 1010};

   
   return 0;
}