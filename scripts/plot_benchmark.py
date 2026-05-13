#!/usr/bin/env python3
"""Builds the sorting benchmark plot from output/benchmark.csv.

The script uses matplotlib when it is available. If it is not installed, it
falls back to a tiny standard-library PNG renderer so the lab remains runnable
on a clean machine.
"""

from __future__ import annotations

import csv
import struct
import zlib
from collections import defaultdict
from pathlib import Path


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
    try:
        plot_with_matplotlib(series, output_path)
    except ImportError:
        plot_with_stdlib_png(series, output_path)
    print(f"Saved {output_path}")
    return 0


def plot_with_matplotlib(series: dict[str, list[tuple[int, float]]], output_path: Path) -> None:
    import matplotlib.pyplot as plt

    plt.figure(figsize=(10, 6))
    for algorithm, points in sorted(series.items()):
        points.sort()
        sizes = [point[0] for point in points]
        milliseconds = [point[1] for point in points]
        plt.plot(sizes, milliseconds, marker="o", label=algorithm)

    plt.title("Sorting algorithms benchmark")
    plt.xlabel("Array size")
    plt.ylabel("Time, ms")
    plt.grid(True, alpha=0.3)
    plt.legend()
    plt.tight_layout()
    plt.savefig(output_path, dpi=160)


def plot_with_stdlib_png(series: dict[str, list[tuple[int, float]]], output_path: Path) -> None:
    width = 1200
    height = 720
    left = 95
    right = 40
    top = 45
    bottom = 95
    colors = {
        "bubble": (230, 126, 34),
        "merge": (52, 152, 219),
        "shaker": (231, 76, 60),
        "std::sort": (155, 89, 182),
    }

    image = bytearray([255, 255, 255] * width * height)
    all_points = [point for points in series.values() for point in points]
    min_size = min(size for size, _ in all_points)
    max_size = max(size for size, _ in all_points)
    max_ms = max(milliseconds for _, milliseconds in all_points)

    def put_pixel(x: int, y: int, color: tuple[int, int, int]) -> None:
        if 0 <= x < width and 0 <= y < height:
            offset = (y * width + x) * 3
            image[offset:offset + 3] = bytes(color)

    def draw_line(x1: int, y1: int, x2: int, y2: int, color: tuple[int, int, int]) -> None:
        dx = abs(x2 - x1)
        dy = -abs(y2 - y1)
        sx = 1 if x1 < x2 else -1
        sy = 1 if y1 < y2 else -1
        error = dx + dy

        while True:
            put_pixel(x1, y1, color)
            if x1 == x2 and y1 == y2:
                break
            doubled_error = 2 * error
            if doubled_error >= dy:
                error += dy
                x1 += sx
            if doubled_error <= dx:
                error += dx
                y1 += sy

    def draw_rect(x1: int, y1: int, x2: int, y2: int, color: tuple[int, int, int]) -> None:
        for y in range(y1, y2 + 1):
            for x in range(x1, x2 + 1):
                put_pixel(x, y, color)

    def scale_x(size: int) -> int:
        span = max_size - min_size
        if span == 0:
            return left
        return left + round((size - min_size) / span * (width - left - right))

    def scale_y(milliseconds: float) -> int:
        if max_ms == 0:
            return height - bottom
        return height - bottom - round(milliseconds / max_ms * (height - top - bottom))

    gray = (220, 220, 220)
    black = (30, 30, 30)
    for tick in range(6):
        y = top + round(tick * (height - top - bottom) / 5)
        draw_line(left, y, width - right, y, gray)
    for tick in range(6):
        x = left + round(tick * (width - left - right) / 5)
        draw_line(x, top, x, height - bottom, gray)

    draw_line(left, top, left, height - bottom, black)
    draw_line(left, height - bottom, width - right, height - bottom, black)

    legend_x = left + 15
    legend_y = top + 15
    for index, (algorithm, points) in enumerate(sorted(series.items())):
        color = colors.get(algorithm, (0, 0, 0))
        points = sorted(points)
        screen_points = [(scale_x(size), scale_y(milliseconds)) for size, milliseconds in points]

        for current, next_point in zip(screen_points, screen_points[1:]):
            draw_line(current[0], current[1], next_point[0], next_point[1], color)
        for x, y in screen_points:
            draw_rect(x - 3, y - 3, x + 3, y + 3, color)

        y = legend_y + index * 18
        draw_rect(legend_x, y, legend_x + 28, y + 8, color)
        # Minimal legend markers; CSV keeps exact algorithm names and values.

    write_png(output_path, width, height, image)


def write_png(path: Path, width: int, height: int, rgb: bytearray) -> None:
    def chunk(kind: bytes, payload: bytes) -> bytes:
        return (
            struct.pack(">I", len(payload))
            + kind
            + payload
            + struct.pack(">I", zlib.crc32(kind + payload) & 0xFFFFFFFF)
        )

    raw = bytearray()
    row_size = width * 3
    for y in range(height):
        raw.append(0)
        start = y * row_size
        raw.extend(rgb[start:start + row_size])

    png = bytearray(b"\x89PNG\r\n\x1a\n")
    png.extend(chunk(b"IHDR", struct.pack(">IIBBBBB", width, height, 8, 2, 0, 0, 0)))
    png.extend(chunk(b"IDAT", zlib.compress(bytes(raw), level=9)))
    png.extend(chunk(b"IEND", b""))
    path.write_bytes(png)


if __name__ == "__main__":
    raise SystemExit(main())
