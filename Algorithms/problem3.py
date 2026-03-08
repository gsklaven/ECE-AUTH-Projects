import numpy as np

def calculate_splitting_cost(prices, num_parts):
    prices = [0] + prices + [num_parts]
    m = len(prices)
    prices = [0] + prices
    costs = np.full((m + 1, m + 1), np.inf)
    for i in range(1, m + 1):
        costs[i, i:i + 2] = 0

    for s in range(2, m):
        for i in range(1, m - s + 1):
            j = i + s
            for k in range(i, j + 1):
                costs[i, j] = min(costs[i, j], prices[j] - prices[i] + costs[i, k] + costs[k, j])
    return costs[1, m]


string = "abcdfedghjg"
print("Length of string:", len(string))
print("Minimum splitting cost:", calculate_splitting_cost([1, 6, 3], len(string)))
