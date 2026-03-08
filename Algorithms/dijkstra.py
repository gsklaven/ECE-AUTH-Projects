def dijkstra(graph, start):
    # Initialize distances to infinity for all nodes except the start node
    distances = {node: float('infinity') for node in graph}
    # Distance from a node to itself is always 0
    distances[start] = 0 #O(1)

    # Keep a dictionary that contains the previous node for each node
    previous_nodes = {node: None for node in graph} #O(V)

    # List of nodes to visit, initially add the start node
    nodes_to_visit = [start] #O(1)

    while nodes_to_visit: #O(V)
        # Choose the node with the smallest distance
        current_node = None #O(1)
        for node in nodes_to_visit: #O(V)
            if current_node is None or distances[node] < distances[current_node]:
                current_node = node #O(1)

        # Remove the node from the list of nodes to visit
        nodes_to_visit.remove(current_node)  #O(V)

        # Explore neighbors of the current node
        for neighbor, weight in graph[current_node].items(): #O(E)
            new_distance = distances[current_node] + weight #O(1)
            # If the new distance is smaller than the stored one, update it
            if new_distance < distances[neighbor]: #O(1)
                distances[neighbor] = new_distance #O(1)
                previous_nodes[neighbor] = current_node #O(1)
                # Add the neighbor to the list to visit if it's not already there
                if neighbor not in nodes_to_visit: #O(V)
                    nodes_to_visit.append(neighbor) #O(1)

    return distances, previous_nodes #O(1)
# 2VV + VE + V + V = VE + 2VV + 2V = V^2 + VE

def shortest_path(graph, start, end):
    distances, previous_nodes = dijkstra(graph, start)

    # If there is no path from start to end, return None
    if distances[end] == float('infinity'):
        return None

    # If there is a path, find the route
    path = []
    current_node = end
    while current_node is not None:
        path.append(current_node)
        current_node = previous_nodes[current_node]
    # Reverse the path for the correct order
    path = path[::-1]

    return path