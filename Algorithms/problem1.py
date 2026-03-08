from dijkstra import dijkstra
from dijkstra import shortest_path

graph = {
    'A': {'B': 2, 'C': 1, 'D': 4},
    'B': {'A': 2, 'C': 3, 'E': 5},
    'C': {'A': 1, 'B': 3, 'D': 2},
    'D': {'A': 4, 'C': 2, 'E': 1},
    'E': {'B': 5, 'D': 1}
}

if __name__ == "__main__":
    print(graph)
    print(graph['A'].values())

    start_node = 'A'
    end_node = 'E'

    shortest_path = shortest_path(graph, start_node, end_node)
    if shortest_path is not None:
        print("Συντομότερη διαδρομή από τον κόμβο", start_node, "στον κόμβο", end_node, ":", shortest_path)
        total_distance = sum(graph[shortest_path[i]][shortest_path[i + 1]] for i in range(len(shortest_path) - 1))
        print("Συνολική απόσταση:", total_distance)
    else:
        print("Δεν υπάρχει διαδρομή από τον κόμβο", start_node, "στον κόμβο", end_node)