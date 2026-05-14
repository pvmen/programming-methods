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

Показать первые строки до и после сортировки:

```bash
./build/sorting_lab preview data/input_1000.csv merge 12
```

Запустить полный бенчмарк:

```bash
./build/sorting_lab benchmark
```

Построить график:

```bash
python3 -m venv .venv
.venv/bin/python -m pip install -r requirements.txt
.venv/bin/python scripts/plot_benchmark.py
```

Собрать PDF-отчет:

```bash
.venv/bin/python scripts/build_report_pdf.py
```

Сгенерировать Doxygen-документацию:

```bash
doxygen Doxyfile
```