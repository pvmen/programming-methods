#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>

const int SAMPLE_COUNT = 20;
const int SAMPLE_SIZE = 1000;
const int MIN_VALUE = 0;
const int MAX_VALUE = 9999;
const int UNIFORM_BINS = 20;
const int INDEPENDENCE_BINS = 10;
const int TIME_REPEAT_COUNT = 100;
const double CHI_SQUARE_UNIFORM_CRITICAL = 30.144;
const double CHI_SQUARE_RANDOM_CRITICAL = 123.225;
const double RANDOMNESS_Z_CRITICAL = 1.96;

/**
 * @brief Статистика одной выборки.
 */
struct SampleStats {
  std::string generatorName; ///< Название генератора.
  int sampleNumber;          ///< Номер выборки.
  double mean;               ///< Среднее значение.
  double deviation;          ///< Среднеквадратичное отклонение.
  double variation;          ///< Коэффициент вариации.
  double uniformChiSquare;   ///< Значение критерия Хи-квадрат для равномерности.
  bool uniformAccepted;      ///< true, если гипотеза о равномерности не отвергается.
  double randomChiSquare;    ///< Значение критерия Хи-квадрат для пар соседних чисел.
  bool randomAccepted;       ///< true, если гипотеза о случайности не отвергается.
};

/**
 * @brief Результат одного теста случайности.
 */
struct RandomTestResult {
  std::string generatorName; ///< Название генератора.
  std::string testName;      ///< Название теста.
  double statistic;          ///< Значение статистики теста.
  double limit;              ///< Критическое значение.
  bool passed;               ///< true, если тест пройден.
};

/**
 * @brief Генератор LCG с дополнительным перемешиванием битов.
 */
struct MixedLCG {
  uint32_t state;

  MixedLCG(uint32_t seed) {
    state = seed;
  }

  uint32_t next() {
    state = 1664525u * state + 1013904223u;

    uint32_t value = state;
    value ^= value >> 16;
    value *= 2246822519u;
    value ^= value >> 13;
    value *= 3266489917u;
    value ^= value >> 16;

    return value;
  }
};

/**
 * @brief Генератор Xorshift с последовательностью Вейля.
 */
struct XorshiftWeyl {
  uint32_t state;
  uint32_t weyl;

  XorshiftWeyl(uint32_t seed) {
    state = seed;
    weyl = seed ^ 0x61c88647u;
  }

  uint32_t next() {
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;
    weyl += 0x61c88647u;

    return state + weyl;
  }
};

/**
 * @brief Аддитивный генератор Фибоначчи с запаздываниями 24 и 55.
 */
struct LaggedFibonacci {
  std::vector<uint32_t> state;
  int index;

  LaggedFibonacci(uint32_t seed) {
    state.resize(55);
    index = 0;

    uint32_t value = seed;
    for (std::size_t i = 0; i < state.size(); i++) {
      value = 1664525u * value + 1013904223u;
      state[i] = value;
    }
  }

  uint32_t next() {
    int j = (index + 31) % 55;
    uint32_t value = state[index] + state[j];
    state[index] = value;
    index = (index + 1) % 55;

    return value;
  }
};

/**
 * @brief Обертка над стандартным генератором языка C++.
 */
struct StandardGenerator {
  std::mt19937 generator;

  StandardGenerator(uint32_t seed) {
    generator.seed(seed);
  }

  uint32_t next() {
    return generator();
  }
};

/**
 * @brief Возвращает число из заданного диапазона.
 * @param value Сырое значение генератора.
 * @param minValue Нижняя граница диапазона.
 * @param maxValue Верхняя граница диапазона.
 * @return Число в диапазоне [minValue; maxValue].
 */
int toRange(uint32_t value, int minValue, int maxValue) {
  return minValue + static_cast<int>(value % static_cast<uint32_t>(maxValue - minValue + 1));
}

/**
 * @brief Формирует выборку заданного размера.
 * @param generator Генератор псевдослучайных чисел.
 * @param size Размер выборки.
 * @return Массив значений выборки.
 */
template <typename Generator>
std::vector<int> generateSample(Generator& generator, int size) {
  std::vector<int> sample;
  sample.reserve(size);

  for (int i = 0; i < size; i++) {
    sample.push_back(toRange(generator.next(), MIN_VALUE, MAX_VALUE));
  }

  return sample;
}

/**
 * @brief Вычисляет среднее значение выборки.
 * @param sample Выборка чисел.
 * @return Среднее значение.
 */
double calculateMean(const std::vector<int>& sample) {
  double sum = 0.0;

  for (std::size_t i = 0; i < sample.size(); i++) {
    sum += sample[i];
  }

  return sum / static_cast<double>(sample.size());
}

/**
 * @brief Вычисляет среднеквадратичное отклонение.
 * @param sample Выборка чисел.
 * @param mean Среднее значение выборки.
 * @return Среднеквадратичное отклонение.
 */
double calculateDeviation(const std::vector<int>& sample, double mean) {
  double sum = 0.0;

  for (std::size_t i = 0; i < sample.size(); i++) {
    double difference = sample[i] - mean;
    sum += difference * difference;
  }

  return std::sqrt(sum / static_cast<double>(sample.size()));
}

/**
 * @brief Проверяет равномерность распределения по критерию Хи-квадрат.
 * @param sample Выборка чисел.
 * @param bins Количество интервалов.
 * @return Значение статистики Хи-квадрат.
 */
double chiSquareUniformity(const std::vector<int>& sample, int bins) {
  std::vector<int> frequencies(bins, 0);
  int range = MAX_VALUE - MIN_VALUE + 1;

  for (std::size_t i = 0; i < sample.size(); i++) {
    int bin = (sample[i] - MIN_VALUE) * bins / range;

    if (bin == bins) {
      bin--;
    }

    frequencies[bin]++;
  }

  double expected = static_cast<double>(sample.size()) / bins;
  double chiSquare = 0.0;

  for (int i = 0; i < bins; i++) {
    double difference = frequencies[i] - expected;
    chiSquare += difference * difference / expected;
  }

  return chiSquare;
}

/**
 * @brief Проверяет частоту пар соседних значений по критерию Хи-квадрат.
 * @param sample Выборка чисел.
 * @param bins Количество интервалов по каждой оси.
 * @return Значение статистики Хи-квадрат.
 */
double chiSquareRandomness(const std::vector<int>& sample, int bins) {
  std::vector<std::vector<int>> frequencies(bins, std::vector<int>(bins, 0));
  int range = MAX_VALUE - MIN_VALUE + 1;

  for (std::size_t i = 1; i < sample.size(); i++) {
    int previousBin = (sample[i - 1] - MIN_VALUE) * bins / range;
    int currentBin = (sample[i] - MIN_VALUE) * bins / range;

    if (previousBin == bins) {
      previousBin--;
    }

    if (currentBin == bins) {
      currentBin--;
    }

    frequencies[previousBin][currentBin]++;
  }

  double expected = static_cast<double>(sample.size() - 1) / (bins * bins);
  double chiSquare = 0.0;

  for (int i = 0; i < bins; i++) {
    for (int j = 0; j < bins; j++) {
      double difference = frequencies[i][j] - expected;
      chiSquare += difference * difference / expected;
    }
  }

  return chiSquare;
}

/**
 * @brief Собирает статистику по выборке.
 * @param generatorName Название генератора.
 * @param sampleNumber Номер выборки.
 * @param sample Выборка чисел.
 * @return Статистика выборки.
 */
SampleStats calculateStats(std::string generatorName, int sampleNumber, const std::vector<int>& sample) {
  SampleStats stats;
  stats.generatorName = generatorName;
  stats.sampleNumber = sampleNumber;
  stats.mean = calculateMean(sample);
  stats.deviation = calculateDeviation(sample, stats.mean);
  stats.variation = stats.deviation / stats.mean;
  stats.uniformChiSquare = chiSquareUniformity(sample, UNIFORM_BINS);
  stats.uniformAccepted = stats.uniformChiSquare < CHI_SQUARE_UNIFORM_CRITICAL;
  stats.randomChiSquare = chiSquareRandomness(sample, INDEPENDENCE_BINS);
  stats.randomAccepted = stats.randomChiSquare < CHI_SQUARE_RANDOM_CRITICAL;

  return stats;
}

/**
 * @brief Преобразует поток чисел генератора в поток битов.
 * @param generator Генератор псевдослучайных чисел.
 * @param bitCount Количество битов.
 * @return Массив битов.
 */
template <typename Generator>
std::vector<int> generateBits(Generator& generator, int bitCount) {
  std::vector<int> bits;
  bits.reserve(bitCount);

  while (static_cast<int>(bits.size()) < bitCount) {
    uint32_t value = generator.next();

    for (int i = 0; i < 32 && static_cast<int>(bits.size()) < bitCount; i++) {
      bits.push_back(static_cast<int>((value >> i) & 1u));
    }
  }

  return bits;
}

/**
 * @brief Выполняет частотный тест NIST.
 * @param generatorName Название генератора.
 * @param bits Массив битов.
 * @return Результат теста.
 */
RandomTestResult monobitTest(std::string generatorName, const std::vector<int>& bits) {
  int sum = 0;

  for (std::size_t i = 0; i < bits.size(); i++) {
    sum += bits[i] == 1 ? 1 : -1;
  }

  double statistic = std::abs(sum) / std::sqrt(static_cast<double>(bits.size()));

  RandomTestResult result;
  result.generatorName = generatorName;
  result.testName = "monobit_frequency";
  result.statistic = statistic;
  result.limit = RANDOMNESS_Z_CRITICAL;
  result.passed = statistic < result.limit;

  return result;
}

/**
 * @brief Выполняет тест серий NIST.
 * @param generatorName Название генератора.
 * @param bits Массив битов.
 * @return Результат теста.
 */
RandomTestResult runsTest(std::string generatorName, const std::vector<int>& bits) {
  int ones = 0;

  for (std::size_t i = 0; i < bits.size(); i++) {
    if (bits[i] == 1) {
      ones++;
    }
  }

  double proportion = static_cast<double>(ones) / bits.size();
  int runs = 1;

  for (std::size_t i = 1; i < bits.size(); i++) {
    if (bits[i] != bits[i - 1]) {
      runs++;
    }
  }

  double expected = 2.0 * bits.size() * proportion * (1.0 - proportion);
  double dispersion = 2.0 * bits.size() * proportion * (1.0 - proportion)
      * (2.0 * bits.size() * proportion * (1.0 - proportion) - 1.0)
      / (bits.size() - 1.0);
  double statistic = std::abs(runs - expected) / std::sqrt(dispersion);

  RandomTestResult result;
  result.generatorName = generatorName;
  result.testName = "runs";
  result.statistic = statistic;
  result.limit = RANDOMNESS_Z_CRITICAL;
  result.passed = statistic < result.limit;

  return result;
}

/**
 * @brief Выполняет блочный частотный тест NIST.
 * @param generatorName Название генератора.
 * @param bits Массив битов.
 * @param blockSize Размер блока.
 * @return Результат теста.
 */
RandomTestResult blockFrequencyTest(std::string generatorName, const std::vector<int>& bits, int blockSize) {
  int blockCount = static_cast<int>(bits.size()) / blockSize;
  double chiSquare = 0.0;

  for (int block = 0; block < blockCount; block++) {
    int ones = 0;

    for (int i = 0; i < blockSize; i++) {
      ones += bits[block * blockSize + i];
    }

    double proportion = static_cast<double>(ones) / blockSize;
    double difference = proportion - 0.5;
    chiSquare += 4.0 * blockSize * difference * difference;
  }

  double expected = blockCount;
  double deviation = std::sqrt(2.0 * blockCount);
  double statistic = std::abs(chiSquare - expected) / deviation;

  RandomTestResult result;
  result.generatorName = generatorName;
  result.testName = "block_frequency";
  result.statistic = statistic;
  result.limit = RANDOMNESS_Z_CRITICAL;
  result.passed = statistic < result.limit;

  return result;
}

/**
 * @brief Выполняет serial test для пар битов.
 * @param generatorName Название генератора.
 * @param bits Массив битов.
 * @return Результат теста.
 */
RandomTestResult serialTest(std::string generatorName, const std::vector<int>& bits) {
  std::vector<int> frequencies(4, 0);

  for (std::size_t i = 1; i < bits.size(); i++) {
    int value = bits[i - 1] * 2 + bits[i];
    frequencies[value]++;
  }

  double expected = static_cast<double>(bits.size() - 1) / 4.0;
  double chiSquare = 0.0;

  for (int i = 0; i < 4; i++) {
    double difference = frequencies[i] - expected;
    chiSquare += difference * difference / expected;
  }

  RandomTestResult result;
  result.generatorName = generatorName;
  result.testName = "serial";
  result.statistic = chiSquare;
  result.limit = 7.815;
  result.passed = chiSquare < result.limit;

  return result;
}

/**
 * @brief Выполняет NIST-тест максимальной длины серии единиц в блоке.
 * @param generatorName Название генератора.
 * @param bits Массив битов.
 * @return Результат теста.
 */
RandomTestResult longestRunOfOnesTest(std::string generatorName, const std::vector<int>& bits) {
  const int blockSize = 128;
  const int categoryCount = 6;
  int blockCount = static_cast<int>(bits.size()) / blockSize;
  std::vector<int> frequencies(categoryCount, 0);
  std::vector<double> probabilities = {
    0.1174035788,
    0.2429559590,
    0.2493634830,
    0.1751770600,
    0.1027010710,
    0.1123988470
  };

  for (int block = 0; block < blockCount; block++) {
    int longestRun = 0;
    int currentRun = 0;

    for (int i = 0; i < blockSize; i++) {
      if (bits[block * blockSize + i] == 1) {
        currentRun++;
        longestRun = std::max(longestRun, currentRun);
      } else {
        currentRun = 0;
      }
    }

    if (longestRun <= 4) {
      frequencies[0]++;
    } else if (longestRun == 5) {
      frequencies[1]++;
    } else if (longestRun == 6) {
      frequencies[2]++;
    } else if (longestRun == 7) {
      frequencies[3]++;
    } else if (longestRun == 8) {
      frequencies[4]++;
    } else {
      frequencies[5]++;
    }
  }

  double chiSquare = 0.0;

  for (int i = 0; i < categoryCount; i++) {
    double expected = blockCount * probabilities[i];
    double difference = frequencies[i] - expected;
    chiSquare += difference * difference / expected;
  }

  RandomTestResult result;
  result.generatorName = generatorName;
  result.testName = "longest_run_of_ones";
  result.statistic = chiSquare;
  result.limit = 11.070;
  result.passed = chiSquare < result.limit;

  return result;
}

/**
 * @brief Выполняет набор тестов NIST/Diehard-like для генератора.
 * @param generatorName Название генератора.
 * @param generator Генератор псевдослучайных чисел.
 * @return Массив результатов тестов.
 */
template <typename Generator>
std::vector<RandomTestResult> runRandomTests(std::string generatorName, Generator& generator) {
  std::vector<int> bits = generateBits(generator, 100000);
  std::vector<RandomTestResult> results;

  results.push_back(monobitTest(generatorName, bits));
  results.push_back(runsTest(generatorName, bits));
  results.push_back(blockFrequencyTest(generatorName, bits, 1000));
  results.push_back(serialTest(generatorName, bits));
  results.push_back(longestRunOfOnesTest(generatorName, bits));

  return results;
}

/**
 * @brief Записывает статистику выборок в файл.
 * @param stats Массив статистик.
 */
void writeSampleStats(const std::vector<SampleStats>& stats) {
  std::ofstream file("data/sample_stats.txt");
  file << "generator sample mean deviation variation uniform_chi uniform_ok random_chi random_ok\n";
  file << std::fixed << std::setprecision(6);

  for (std::size_t i = 0; i < stats.size(); i++) {
    file << stats[i].generatorName << " "
         << stats[i].sampleNumber << " "
         << stats[i].mean << " "
         << stats[i].deviation << " "
         << stats[i].variation << " "
         << stats[i].uniformChiSquare << " "
         << (stats[i].uniformAccepted ? "yes" : "no") << " "
         << stats[i].randomChiSquare << " "
         << (stats[i].randomAccepted ? "yes" : "no") << "\n";
  }
}

/**
 * @brief Записывает результаты тестов случайности в файл.
 * @param results Массив результатов.
 */
void writeRandomTests(const std::vector<RandomTestResult>& results) {
  std::ofstream file("data/random_tests.txt");
  file << "generator test statistic limit passed\n";
  file << std::fixed << std::setprecision(6);

  for (std::size_t i = 0; i < results.size(); i++) {
    file << results[i].generatorName << " "
         << results[i].testName << " "
         << results[i].statistic << " "
         << results[i].limit << " "
         << (results[i].passed ? "yes" : "no") << "\n";
  }
}

/**
 * @brief Измеряет время генерации чисел.
 * @param generator Генератор псевдослучайных чисел.
 * @param size Количество чисел.
 * @return Время генерации в наносекундах.
 */
template <typename Generator>
auto measureGenerationTime(Generator& generator, int size) {
  volatile uint32_t sink = 0;
  auto start = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < size; i++) {
    sink ^= generator.next();
  }

  auto end = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
}

/**
 * @brief Выполняет замеры скорости генераторов.
 */
void measureAllGenerators() {
  std::vector<int> sizes = {
    1000, 5000, 10000, 50000, 100000, 250000, 500000, 1000000
  };

  std::ofstream file("data/generation_times.txt");
  file << "generator size nanoseconds\n";

  for (std::size_t i = 0; i < sizes.size(); i++) {
    int size = sizes[i];

    MixedLCG lcg(101u);
    XorshiftWeyl xorshift(202u);
    LaggedFibonacci fibonacci(303u);
    StandardGenerator standard(404u);

    auto lcgTime = measureGenerationTime(lcg, size * TIME_REPEAT_COUNT) / TIME_REPEAT_COUNT;
    auto xorshiftTime = measureGenerationTime(xorshift, size * TIME_REPEAT_COUNT) / TIME_REPEAT_COUNT;
    auto fibonacciTime = measureGenerationTime(fibonacci, size * TIME_REPEAT_COUNT) / TIME_REPEAT_COUNT;
    auto standardTime = measureGenerationTime(standard, size * TIME_REPEAT_COUNT) / TIME_REPEAT_COUNT;

    file << "mixed_lcg" << " " << size << " " << lcgTime << "\n";
    file << "xorshift_weyl" << " " << size << " " << xorshiftTime << "\n";
    file << "lagged_fibonacci" << " " << size << " " << fibonacciTime << "\n";
    file << "std_mt19937" << " " << size << " " << standardTime << "\n";
  }
}

/**
 * @brief Выводит краткий итог по выборкам одного генератора.
 * @param stats Статистика всех выборок.
 * @param generatorName Название генератора.
 */
void printGeneratorSummary(const std::vector<SampleStats>& stats, std::string generatorName) {
  int uniformPassed = 0;
  int randomPassed = 0;

  for (std::size_t i = 0; i < stats.size(); i++) {
    if (stats[i].generatorName == generatorName) {
      if (stats[i].uniformAccepted) {
        uniformPassed++;
      }

      if (stats[i].randomAccepted) {
        randomPassed++;
      }
    }
  }

  std::cout << generatorName << ": uniform "
            << uniformPassed << "/" << SAMPLE_COUNT
            << ", random "
            << randomPassed << "/" << SAMPLE_COUNT << "\n";
}

/**
 * @brief Точка входа программы.
 *
 * Генерирует по 20 выборок тремя методами ГПСЧ, считает статистики,
 * проверяет равномерность и случайность критерием Хи-квадрат, выполняет
 * пять тестов NIST/Diehard-like и замеряет скорость генерации.
 *
 * @return Код завершения программы.
 */
int main() {
  std::vector<SampleStats> allStats;
  std::vector<RandomTestResult> allRandomTests;

  for (int sampleNumber = 1; sampleNumber <= SAMPLE_COUNT; sampleNumber++) {
    MixedLCG lcg(1000u + sampleNumber);
    XorshiftWeyl xorshift(2000u + sampleNumber);
    LaggedFibonacci fibonacci(3000u + sampleNumber);

    std::vector<int> lcgSample = generateSample(lcg, SAMPLE_SIZE);
    std::vector<int> xorshiftSample = generateSample(xorshift, SAMPLE_SIZE);
    std::vector<int> fibonacciSample = generateSample(fibonacci, SAMPLE_SIZE);

    allStats.push_back(calculateStats("mixed_lcg", sampleNumber, lcgSample));
    allStats.push_back(calculateStats("xorshift_weyl", sampleNumber, xorshiftSample));
    allStats.push_back(calculateStats("lagged_fibonacci", sampleNumber, fibonacciSample));
  }

  MixedLCG lcgForTests(12345u);
  XorshiftWeyl xorshiftForTests(23456u);
  LaggedFibonacci fibonacciForTests(34567u);

  std::vector<RandomTestResult> lcgTests = runRandomTests("mixed_lcg", lcgForTests);
  std::vector<RandomTestResult> xorshiftTests = runRandomTests("xorshift_weyl", xorshiftForTests);
  std::vector<RandomTestResult> fibonacciTests = runRandomTests("lagged_fibonacci", fibonacciForTests);

  allRandomTests.insert(allRandomTests.end(), lcgTests.begin(), lcgTests.end());
  allRandomTests.insert(allRandomTests.end(), xorshiftTests.begin(), xorshiftTests.end());
  allRandomTests.insert(allRandomTests.end(), fibonacciTests.begin(), fibonacciTests.end());

  writeSampleStats(allStats);
  writeRandomTests(allRandomTests);
  measureAllGenerators();

  printGeneratorSummary(allStats, "mixed_lcg");
  printGeneratorSummary(allStats, "xorshift_weyl");
  printGeneratorSummary(allStats, "lagged_fibonacci");

  std::cout << "results saved to data/sample_stats.txt, data/random_tests.txt, data/generation_times.txt\n";

  return 0;
}