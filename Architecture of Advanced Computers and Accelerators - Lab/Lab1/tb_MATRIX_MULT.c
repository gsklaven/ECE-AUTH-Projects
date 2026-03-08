#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#define MAX_VAL 255

#define LM 6
#define LN 6
#define LP 6

#define M (1 << LM)
#define N (1 << LN)
#define P (1 << LP)

void matrixMultiplication(uint8_t A[N * M], uint8_t B[M * P], uint32_t AB[N * P]);

void softwareMatrixMultiplication(uint8_t A[N * M], uint8_t B[M * P], uint32_t AB_sw[N * P]) {
			for (int i = 0; i < N; i++) {
				for (int j = 0; j < P; j++) {
					AB_sw[(i * P) + j] = 0;
					for (int k = 0; k < M; k++) {
						AB_sw[(i * P) + j]  += A[(i * M) + k] * B[(k * P) + j];
					}
				}
			}
		}

int main() {
    uint8_t A[N * M];
    uint8_t B[M * P];
    uint32_t AB_hw[N * P] = {0};
    uint32_t AB_sw[N * P] = {0};
    int i, j;

    srand(0);

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            A[i * M + j] = rand() % (MAX_VAL + 1);
        }
    }

    for (i = 0; i < M; i++) {
        for (j = 0; j < P; j++) {
            B[i * P + j] = rand() % (MAX_VAL + 1);
        }
    }

    softwareMatrixMultiplication(A, B, AB_sw);

    matrixMultiplication(A, B, AB_hw);

    int test_passed = 1;
    for (i = 0; i < N; i++) {
        for (j = 0; j < P; j++) {
            if (AB_sw[i * P + j] != AB_hw[i * P + j]) {
                test_passed = 0;
                printf("Mismatch at [%d][%d]: AB_sw = %d, AB_hw = %d\n", i, j, AB_sw[i * P + j], AB_hw[i * P + j]);
            }
            else if (AB_sw[i * P + j] == AB_hw[i * P + j]) {
                printf("At [%d][%d]: AB_sw = %d, AB_hw = %d\n", i, j, AB_sw[i * P + j], AB_hw[i * P + j]);
            }
        }
    }

    if (test_passed) {
        printf("Test passed\n");
    } else {
        printf("Test failed\n");
    }

    return 0;
}
