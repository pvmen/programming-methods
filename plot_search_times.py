import matplotlib.pyplot as plt

sizes = []
linear_times = []
tree_times = []
hash_times = []
map_times = []
rb_times = []
collisions = []

with open("data/search_times.txt", "r", encoding="utf-8") as file:
    next(file)

    for line in file:
        size, linear, tree, hash_table, multimap, red_black, collision_count = line.split()

        sizes.append(int(size))
        linear_times.append(int(linear))
        tree_times.append(int(tree))
        hash_times.append(int(hash_table))
        map_times.append(int(multimap))
        rb_times.append(int(red_black))
        collisions.append(int(collision_count))

plt.figure(figsize=(10, 6))
plt.plot(sizes, linear_times, marker="o", label="Linear search")
plt.plot(sizes, tree_times, marker="o", label="Binary tree")
plt.plot(sizes, hash_times, marker="o", label="Hash table")
plt.plot(sizes, map_times, marker="o", label="Multimap")
plt.plot(sizes, rb_times, marker="o", label="Red-black tree")

plt.xlabel("Количество записей")
plt.ylabel("Время, нс")
plt.title("Сравнение времени поиска")
plt.legend()
plt.grid(True)
plt.savefig("search_times.png")

plt.figure(figsize=(10, 6))
plt.plot(sizes, collisions, marker="o", label="Hash collisions")

plt.xlabel("Количество записей")
plt.ylabel("Количество коллизий")
plt.title("Количество коллизий в хеш-таблице")
plt.legend()
plt.grid(True)
plt.savefig("hash_collisions.png")