import os
from pathlib import Path

cache_dir = Path(".cache")
(cache_dir / "matplotlib").mkdir(parents=True, exist_ok=True)
(cache_dir / "fontconfig").mkdir(parents=True, exist_ok=True)

os.environ.setdefault("MPLCONFIGDIR", str(cache_dir / "matplotlib"))
os.environ.setdefault("XDG_CACHE_HOME", str(cache_dir))

import matplotlib

matplotlib.use("Agg")

import matplotlib.pyplot as plt


generator_names = {
    "mixed_lcg": "Mixed LCG",
    "xorshift_weyl": "Xorshift Weyl",
    "lagged_fibonacci": "Lagged Fibonacci",
    "std_mt19937": "std::mt19937",
}

times_by_generator = {}
size_labels = {
    1000: "1 тыс.",
    5000: "5 тыс.",
    10000: "10 тыс.",
    50000: "50 тыс.",
    100000: "100 тыс.",
    250000: "250 тыс.",
    500000: "500 тыс.",
    1000000: "1 млн",
}

with open("data/generation_times.txt", "r", encoding="utf-8") as file:
    next(file)

    for line in file:
        generator, size, nanoseconds = line.split()

        if generator not in times_by_generator:
            times_by_generator[generator] = {
                "sizes": [],
                "times": [],
            }

        times_by_generator[generator]["sizes"].append(int(size))
        times_by_generator[generator]["times"].append(int(nanoseconds))

plt.figure(figsize=(11, 6))

for generator, values in times_by_generator.items():
    plt.plot(
        values["sizes"],
        [time / 1000 for time in values["times"]],
        marker="o",
        label=generator_names[generator],
    )

plt.xlabel("Количество сгенерированных чисел")
plt.ylabel("Время генерации, мкс")
plt.title("Время генерации псевдослучайных чисел")
plt.xscale("log")
plt.xticks(list(size_labels.keys()), list(size_labels.values()))
plt.legend()
plt.grid(True, which="both", linestyle="--", alpha=0.5)
plt.tight_layout()
plt.savefig("generation_times.png")

plt.figure(figsize=(11, 6))

for generator, values in times_by_generator.items():
    speeds = []

    for size, time in zip(values["sizes"], values["times"]):
        speeds.append(size / (time / 1_000_000_000) / 1_000_000)

    plt.plot(
        values["sizes"],
        speeds,
        marker="o",
        label=generator_names[generator],
    )

plt.xlabel("Количество сгенерированных чисел")
plt.ylabel("Скорость, млн чисел/с")
plt.title("Скорость генерации псевдослучайных чисел")
plt.xscale("log")
plt.xticks(list(size_labels.keys()), list(size_labels.values()))
plt.legend()
plt.grid(True, which="both", linestyle="--", alpha=0.5)
plt.tight_layout()
plt.savefig("generation_speed.png")

plt.figure(figsize=(11, 6))

for generator, values in times_by_generator.items():
    plt.plot(
        values["sizes"],
        [time / 1000 for time in values["times"]],
        marker="o",
        label=generator_names[generator],
    )

plt.xlabel("Количество сгенерированных чисел")
plt.ylabel("Время генерации, мкс")
plt.title("Время генерации псевдослучайных чисел в логарифмическом масштабе")
plt.xscale("log")
plt.yscale("log")
plt.xticks(list(size_labels.keys()), list(size_labels.values()))
plt.legend()
plt.grid(True, which="both", linestyle="--", alpha=0.5)
plt.tight_layout()
plt.savefig("generation_times_loglog.png")
