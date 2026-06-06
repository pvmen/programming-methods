
import matplotlib.pyplot as plt

times = {}

with open("data/times.txt", "r") as file:
    for line in file:
        size_text, algorithm, time_text = line.split()

        size = int(size_text)
        time = int(time_text)

        if algorithm not in times:
            times[algorithm] = {"sizes": [], "times": []}

        times[algorithm]["sizes"].append(size)
        times[algorithm]["times"].append(time)

for algorithm, values in times.items():
    plt.plot(values["sizes"], values["times"], marker="o", label=algorithm)

skipped_path = Path("data/skipped_times.txt")
if skipped_path.exists():
    skipped_sizes = []
    with skipped_path.open("r") as file:
        for line in file:
            size_text, algorithm, status, reason = line.split()
            skipped_sizes.append(int(size_text))

    if skipped_sizes:
        first_skipped_size = min(skipped_sizes)
        plt.axvline(first_skipped_size, color="gray", linestyle="--", linewidth=1)
        plt.text(
            first_skipped_size,
            plt.ylim()[0],
            "bubble/shaker skipped after this size",
            rotation=90,
            verticalalignment="bottom",
            fontsize=8,
        )

plt.xlabel("Number of products")
plt.ylabel("Time, microseconds")
plt.title("Sorting algorithms comparison")
plt.xscale("log")
plt.yscale("log")
plt.legend()
plt.grid(True)
plt.tight_layout()

plt.savefig("data/times_plot.png", dpi=200)