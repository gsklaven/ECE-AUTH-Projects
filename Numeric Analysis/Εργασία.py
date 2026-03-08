import numpy as np
A = np.array([[1, 2, 3], [4, 5, 6], [7, 8, 9]])

def LUmine(A) :
    n = A.shape[0]
    L = np.matrix( np.identity(n) )
    U = A
    for j in range(0, n-1):
        for i in range (j+1, n) :
            mult = A[i,j] / A[j,j]
            A[i, j+1:n] = A[i,j+1:n] - mult * A[j,j+1:n]
            U[i, j+1:n] = A[i, j+1:n]           
            L[i,j] = mult
            U[i,j] = 0
    return L, U

def QRmine(A):
    n = A.shape[0]
    Q = np.matrix(np.zeros((n, n)))
    R = np.matrix(np.zeros((n, n)))
    for j in range(n):
        q = A[:, j]
        for i in range(j):
            length_of_leg = np.sum(A[:, j].T * Q[:, i])
            q = q - length_of_leg * Q[:, i]
            #R[i, j] = length_of_leg
        q_norm = np.linalg.norm(q)
        Q[:, j] = q / np.linalg.norm(q)
        R[j, j] = q_norm
    return Q, R

L, U = LUmine(A)
Q, R = QRmine(A)
diff = np.linalg.norm(A - Q * R)
print("Difference:", diff)