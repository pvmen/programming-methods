#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <chrono>

struct Product {
  std::string name;
  std::string country; // куда экспорт
  int volume; // объем поставляемой продукции 
  double rubles;
};

bool lessProduct(Product& left, Product& right) {
  if (left.name < right.name) {
    return true;
  }
  if (left.name > right.name) {
    return false;
  } 

  if (left.volume < right.volume) {
    return true;
  } 
  if (left.volume > right.volume) {
    return false;
  } 

  if (left.country < right.country) {
    return true;
  }
  return false;
}

void printProduct(Product& product) {
  std::cout << product.name << " " << product.country << " " << product.volume << " " << product.rubles << "\n";
}

void writeProductsToFile(std::string filename, std::vector<Product>& products){
  std::ofstream file(filename);
  for (std::size_t i = 0; i < products.size(); i++){
    file << products[i].name << " " << products[i].country << " " << products[i].volume << " " << products[i].rubles << "\n";
  }
  file.close();
}

std::vector<Product> readProductsFromFile(std::string filename){
  std::vector<Product> products;
  std::ifstream file(filename);

  Product product;
  while(file >> product.name >> product.country >> product.volume >> product.rubles){
    products.push_back(product);
  }
  return products;
}

void printProducts(std::vector<Product>& products) {
  for (std::size_t i = 0; i < products.size(); i++){
    printProduct(products[i]);
  }
}

void bubbleSort(std::vector<Product>& products) {
  for (std::size_t i = 0; i < products.size(); i++){
    for (std::size_t j = 0; j < products.size() - 1; j++){
      if (lessProduct(products[j+1], products[j])){
        std::swap(products[j+1], products[j]);
      }
    }
  }
}

void shekerSort(std::vector<Product>& products){
  if (products.size() < 2){
    return;
  }
  int left = 0;
  int right = products.size() - 1;
  while (left < right){
    for (int i = left; i < right; i++){    
      if (lessProduct(products[i+1], products[i])){
        std::swap(products[i+1], products[i]);  
      }
    }
    right--;
    for (int j = right; j > left; j--){
      if (lessProduct(products[j], products[j-1])){
        std::swap(products[j-1], products[j]);  
      }
    }
    left++;
  }
}

void merge(std::vector<Product>& products, int left, int middle, int right){
  std::vector<Product> temp;
  int i = left;
  int j = middle + 1;
  while (i <= middle && j <= right){
    if (lessProduct(products[j], products[i])){
    temp.push_back(products[j]);
    j++;
    } else {
    temp.push_back(products[i]);
    i++;
    }
  }
  while (i <= middle){
    temp.push_back(products[i]);
    i++;
  }
  while (j <= right){
    temp.push_back(products[j]);
    j++;
  }
  for (std::size_t k = 0; k < temp.size(); k++){
    products[left + k] = temp[k];
  }
}


void mergeSort(std::vector<Product>& products, int left, int right){
  if (left < right){

    int middle = (left + right) / 2;

    mergeSort(products, left, middle);
    mergeSort(products, middle + 1, right);

    merge(products, left, middle, right);
  }
  return;
}

void mergeSort(std::vector<Product>& products){
  if (products.size() < 2){
    return;
  }
  mergeSort(products, 0, products.size() - 1);
}

int main() {
  // std::vector<Product> products {
  //  {"Oil", "China", 300, 150000.0},
  //  {"Gas", "Turkey", 100, 90000.0},
  //  {"Oil", "Armenia", 200, 100000.0},
  //  {"Oil", "Belarus", 200, 120000.0},
  // };
  std::vector<Product> products = readProductsFromFile("data/input.txt");
  
   //std::cout << "before sort: " << "\n";
   // printProduct(products[0]);
   //printProducts(products);
   //mergeSort(products);
   //writeProductsToFile("data/output.txt", products);
   //std::cout << "after sort: " << "\n";
   //printProducts(products);
   
   std::vector<Product> bubbleProducts = products;
   std::vector<Product> shakerProducts = products;
   std::vector<Product> mergeProducts = products;
   std::vector<Product> stdSortProducts = products;
   // времена
   std::ofstream timeFile("data/times.txt");

   auto start = std::chrono::high_resolution_clock::now();
   bubbleSort(bubbleProducts);
   auto end = std::chrono::high_resolution_clock::now();
   auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
   std::cout << "Bubble sort time: " << duration.count() << " microseconds\n";
   timeFile << "Bubble sort time: " << duration.count() << "\n";

   start = std::chrono::high_resolution_clock::now();
   shekerSort(shakerProducts);
   end = std::chrono::high_resolution_clock::now();
   duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
   std::cout << "shekerSort sort time: " << duration.count() << " microseconds\n";
   timeFile << "shekerSort sort time: " << duration.count() << "\n";


   start = std::chrono::high_resolution_clock::now();
   mergeSort(mergeProducts);
   end = std::chrono::high_resolution_clock::now();
   duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
   std::cout << "mergeSort sort time: " << duration.count() << " microseconds\n";
   timeFile << "mergeSort sort time: " << duration.count() << "\n";

   start = std::chrono::high_resolution_clock::now();
   std::sort(stdSortProducts.begin(), stdSortProducts.end(), lessProduct);
   end = std::chrono::high_resolution_clock::now();
   duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
   std::cout << "std::sort sort time: " << duration.count() << " microseconds\n";
   timeFile << "std::sort sort time: " << duration.count() << "\n";


   writeProductsToFile("data/output_bubble.txt", bubbleProducts);
   writeProductsToFile("data/output_shaker.txt", shakerProducts);
   writeProductsToFile("data/output_merge.txt", mergeProducts);
   writeProductsToFile("data/output_stdSort.txt", stdSortProducts);

   return 0;
}

