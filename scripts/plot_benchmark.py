#!/usr/bin/env python3
"""Builds the sorting benchmark plot from output/benchmark.csv."""

from __future__ import annotations

import csv
from collections import defaultdict
from pathlib import Path

import matplotlib.pyplot as plt
from matplotlib.ticker import FuncFormatter


ALGORITHM_STYLES = {
    "bubble": {
        "label": "Пузырек",
        "color": "#1f77b4",
        "marker": "o",
        "linestyle": "-",
        "text_offset": 12,
    },
    "shaker": {
        "label": "Шейкер",
        "color": "#d62728",
        "marker": "s",
        "linestyle": "--",
        "text_offset": -14,
    },
    "merge": {
        "label": "Слияние",
        "color": "#2ca02c",
        "marker": "^",
        "linestyle": "-.",
        "text_offset": 10,
    },
    "std::sort": {
        "label": "std::sort",
        "color": "#9467bd",
        "marker": "D",
        "linestyle": ":",
        "text_offset": -14,
    },
}


def main() -> int:
    root = Path(__file__).resolve().parents[1]
    benchmark_path = root / "output" / "benchmark.csv"
    output_path = root / "output" / "benchmark.png"

    if not benchmark_path.exists():
        raise SystemExit(f"{benchmark_path} does not exist. Run ./build/sorting_lab benchmark first.")

    series: dict[str, list[tuple[int, float]]] = defaultdict(list)

    with benchmark_path.open(newline="", encoding="utf-8") as benchmark_file:
        reader = csv.DictReader(benchmark_file)
        for row in reader:
            series[row["algorithm"]].append(
                (int(row["size"]), float(row["milliseconds"]))
            )

    output_path.parent.mkdir(parents=True, exist_ok=True)
    plot_benchmark(series, output_path)
    print(f"Saved {output_path}")
    return 0


def plot_benchmark(series: dict[str, list[tuple[int, float]]], output_path: Path) -> None:
    fig, ax = plt.subplots(figsize=(13, 7))

    for algorithm in ALGORITHM_STYLES:
        points = series.get(algorithm)
        if not points:
            continue

        style = ALGORITHM_STYLES[algorithm]
        points.sort()
        sizes = [point[0] for point in points]
        milliseconds = [point[1] for point in points]

        ax.plot(
            sizes,
            milliseconds,
            color=style["color"],
            linestyle=style["linestyle"],
            linewidth=2.4,
            marker=style["marker"],
            markersize=7,
            label=style["label"],
        )

        ax.annotate(
            style["label"],
            xy=(sizes[-1], milliseconds[-1]),
            xytext=(10, style["text_offset"]),
            textcoords="offset points",
            color=style["color"],
            fontsize=10,
            fontweight="bold",
            bbox={"boxstyle": "round,pad=0.25", "fc": "white", "ec": style["color"], "alpha": 0.9},
        )

    ax.set_title("Зависимость времени сортировки от размера массива", fontsize=15, pad=14)
    ax.set_xlabel("Ось X: размер массива, количество записей", fontsize=12)
    ax.set_ylabel("Ось Y: время сортировки, миллисекунды", fontsize=12)
    ax.xaxis.set_major_formatter(FuncFormatter(lambda value, _: f"{int(value):,}".replace(",", " ")))
    ax.yaxis.set_major_formatter(FuncFormatter(lambda value, _: f"{value:g}"))
    ax.grid(True, which="major", linestyle="--", linewidth=0.7, alpha=0.45)
    ax.legend(title="Алгоритм сортировки", loc="upper left", frameon=True)
    ax.margins(x=0.08, y=0.12)
    fig.text(
        0.5,
        0.01,
        "Чем ниже линия, тем быстрее алгоритм на данном размере массива.",
        ha="center",
        fontsize=10,
    )
    fig.tight_layout(rect=(0, 0.04, 1, 1))
    fig.savefig(output_path, dpi=180)
    plt.close(fig)


if __name__ == "__main__":
    raise SystemExit(main())
