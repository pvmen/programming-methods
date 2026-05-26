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

plt.xlabel("Number of products")
plt.ylabel("Time, microseconds")
plt.title("Sorting algorithms comparison")
plt.yscale("log")
plt.legend()
plt.grid(True)

plt.savefig("data/times_plot.png")