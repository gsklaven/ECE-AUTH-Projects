# Input constants
L = 0
R = 20
BreakPoints = [10,18,19]

# Fill the arrays
NearestRight = {k: R for k in range(L, R + 1)}
NearestLeft = {k: L for k in range(L, R + 1)}

# Calculating cost
cost = 0
for BreakPoint in BreakPoints:
    left = NearestLeft[BreakPoint]
    right = NearestRight[BreakPoint]
    cost += (right - left)
    for k in range(left + 1, BreakPoint):
        NearestRight[k] = BreakPoint
    for k in range(BreakPoint + 1, right):
        NearestLeft[k] = BreakPoint

print(cost)  # Outputs 25
