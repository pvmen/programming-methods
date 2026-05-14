#!/usr/bin/env python3
"""Builds report/report.pdf for the sorting lab."""

from __future__ import annotations

import csv
from pathlib import Path

from reportlab.lib import colors
from reportlab.lib.pagesizes import A4
from reportlab.lib.styles import ParagraphStyle, getSampleStyleSheet
from reportlab.lib.units import cm
from reportlab.pdfbase import pdfmetrics
from reportlab.pdfbase.ttfonts import TTFont
from reportlab.platypus import (
    Image,
    Paragraph,
    SimpleDocTemplate,
    Spacer,
    Table,
    TableStyle,
)


def main() -> int:
    root = Path(__file__).resolve().parents[1]
    output_path = root / "report" / "report.pdf"
    benchmark_path = root / "output" / "benchmark.csv"
    plot_path = root / "output" / "benchmark.png"

    register_font()
    styles = build_styles()

    story = [
        Paragraph("Отчет по практической работе 1", styles["Title"]),
        Paragraph("Алгоритмы сортировок. Вариант 9", styles["Subtitle"]),
        Spacer(1, 0.5 * cm),
        Paragraph("Вариант", styles["Heading2"]),
        Paragraph(
            "Массив данных об экспортируемых товарах: наименование товара, страна, "
            "объем поставляемой продукции, сумма в рублях.",
            styles["Body"],
        ),
        Paragraph(
            "Сравнение выполняется по полям: наименование товара, затем объем, затем страна. "
            "Сумма в рублях хранится в записи, но не участвует в сравнении.",
            styles["Body"],
        ),
        Paragraph("Реализованные сортировки", styles["Heading2"]),
        bullet("Сортировка пузырьком.", styles),
        bullet("Шейкер-сортировка.", styles),
        bullet("Сортировка слиянием.", styles),
        bullet("std::sort используется как эталон стандартной библиотеки.", styles),
        Paragraph("Исходный код", styles["Heading2"]),
        Paragraph(
            'Репозиторий: <link href="https://github.com/pvmen/programming-methods">'
            "https://github.com/pvmen/programming-methods</link>",
            styles["Body"],
        ),
        Paragraph("Документация к коду", styles["Heading2"]),
        Paragraph(
            "Документация сгенерирована с помощью Doxygen. HTML-документация находится в "
            "docs/html/index.html. Команда генерации: doxygen Doxyfile.",
            styles["Body"],
        ),
        Paragraph("Входные и выходные данные", styles["Heading2"]),
        Paragraph(
            "Входные данные читаются из CSV-файлов в папке data для каждого размера массива. "
            "После сортировки результаты записываются в CSV-файлы в папке output. "
            "Замеры времени сохраняются в файл benchmark.csv.",
            styles["Body"],
        ),
        Paragraph("График времени сортировок", styles["Heading2"]),
    ]

    if plot_path.exists():
        story.append(Image(str(plot_path), width=17 * cm, height=9.56 * cm))
        story.append(Spacer(1, 0.3 * cm))

    story.extend(
        [
            Paragraph(
                "Для графика используются 10 размеров: 100, 250, 500, 1000, 2500, "
                "5000, 10000, 25000, 50000, 110000. Данные блочно перемешаны: "
                "внутри каждого блока элементы идут в обратном порядке.",
                styles["Body"],
            ),
            Paragraph("Итоговые замеры", styles["Heading2"]),
            build_benchmark_table(benchmark_path),
            Paragraph("Выводы", styles["Heading2"]),
            Paragraph(
                "Пузырек и шейкер-сортировка имеют квадратичную сложность O(n^2), "
                "поэтому на больших перемешанных массивах их время растет резко. "
                "Сортировка слиянием и std::sort масштабируются лучше, так как работают "
                "за O(n log n). Сортировка слиянием стабильна, но требует дополнительную "
                "память O(n).",
                styles["Body"],
            ),
        ]
    )

    output_path.parent.mkdir(parents=True, exist_ok=True)
    document = SimpleDocTemplate(
        str(output_path),
        pagesize=A4,
        rightMargin=1.5 * cm,
        leftMargin=1.5 * cm,
        topMargin=1.5 * cm,
        bottomMargin=1.5 * cm,
        title="Отчет по практической работе 1",
    )
    document.build(story)
    print(f"Saved {output_path}")
    return 0


def register_font() -> None:
    font_path = Path(__file__).resolve().parents[1] / ".venv" / "lib"
    candidates = list(font_path.glob("python*/site-packages/matplotlib/mpl-data/fonts/ttf/DejaVuSans.ttf"))
    if not candidates:
        candidates = [Path("/System/Library/Fonts/Supplemental/Arial Unicode.ttf")]
    pdfmetrics.registerFont(TTFont("ReportFont", str(candidates[0])))


def build_styles() -> dict[str, ParagraphStyle]:
    base = getSampleStyleSheet()
    return {
        "Title": ParagraphStyle(
            "Title",
            parent=base["Title"],
            fontName="ReportFont",
            fontSize=18,
            leading=22,
            spaceAfter=8,
        ),
        "Subtitle": ParagraphStyle(
            "Subtitle",
            parent=base["Normal"],
            fontName="ReportFont",
            fontSize=13,
            leading=16,
            alignment=1,
            spaceAfter=12,
        ),
        "Heading2": ParagraphStyle(
            "Heading2",
            parent=base["Heading2"],
            fontName="ReportFont",
            fontSize=13,
            leading=16,
            spaceBefore=10,
            spaceAfter=6,
        ),
        "Body": ParagraphStyle(
            "Body",
            parent=base["BodyText"],
            fontName="ReportFont",
            fontSize=10,
            leading=13,
            spaceAfter=6,
        ),
    }


def bullet(text: str, styles: dict[str, ParagraphStyle]) -> Paragraph:
    return Paragraph(f"• {text}", styles["Body"])


def build_benchmark_table(path: Path) -> Table:
    rows = [["Размер", "Пузырек, мс", "Шейкер, мс", "Слияние, мс", "std::sort, мс"]]
    values: dict[int, dict[str, float]] = {}

    with path.open(newline="", encoding="utf-8") as benchmark_file:
        reader = csv.DictReader(benchmark_file)
        for row in reader:
            size = int(row["size"])
            values.setdefault(size, {})[row["algorithm"]] = float(row["milliseconds"])

    for size in sorted(values):
        row = values[size]
        rows.append(
            [
                format_size(size),
                format_ms(row["bubble"]),
                format_ms(row["shaker"]),
                format_ms(row["merge"]),
                format_ms(row["std::sort"]),
            ]
        )

    table = Table(rows, repeatRows=1)
    table.setStyle(
        TableStyle(
            [
                ("FONTNAME", (0, 0), (-1, -1), "ReportFont"),
                ("FONTSIZE", (0, 0), (-1, -1), 8),
                ("BACKGROUND", (0, 0), (-1, 0), colors.lightgrey),
                ("GRID", (0, 0), (-1, -1), 0.25, colors.grey),
                ("ALIGN", (1, 1), (-1, -1), "RIGHT"),
                ("VALIGN", (0, 0), (-1, -1), "MIDDLE"),
                ("BOTTOMPADDING", (0, 0), (-1, -1), 4),
                ("TOPPADDING", (0, 0), (-1, -1), 4),
            ]
        )
    )
    return table


def format_size(size: int) -> str:
    return f"{size:,}".replace(",", " ")


def format_ms(value: float) -> str:
    return f"{value:.3f}"


if __name__ == "__main__":
    raise SystemExit(main())
