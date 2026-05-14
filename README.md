# programming-methods

Практическая работа 1 по теме "Алгоритмы сортировок".

## Вариант

Вариант 9: массив данных об экспортируемых товарах.

Поля:

- наименование товара;
- страна, куда экспортируется товар;
- объем поставляемой продукции;
- сумма в рублях.

Сравнение выполняется по ключу: наименование товара, объем поставляемой продукции, страна.

Алгоритмы:

- сортировка пузырьком;
- шейкер-сортировка;
- сортировка слиянием;
- `std::sort` для сравнения.

## Сборка

```bash
cmake -S . -B build
cmake --build build
```

## Запуск

Сгенерировать CSV-файлы для всех размеров:

```bash
./build/sorting_lab generate
```

Отсортировать один файл:

```bash
./build/sorting_lab sort data/input_1000.csv output/sorted_merge_1000.csv merge
```

Запустить полный бенчмарк:

```bash
./build/sorting_lab benchmark
```

Построить график:

```bash
python3 -m pip install -r requirements.txt
python3 scripts/plot_benchmark.py
```

Сгенерировать Doxygen-документацию:

```bash
doxygen Doxyfile
```