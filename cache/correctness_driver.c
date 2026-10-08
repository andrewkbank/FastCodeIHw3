// Generated with GPT-5.6 Sol.
#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "transposes.h"

/*
 * Interleave row/column bits to produce a Morton index.
 */
static size_t morton_index(unsigned row, unsigned col)
{
    size_t z = 0;

    for (unsigned bit = 0; bit < sizeof(unsigned) * 4; ++bit)
    {
        z |= ((size_t)((col >> bit) & 1U)) << (2 * bit);
        z |= ((size_t)((row >> bit) & 1U)) << (2 * bit + 1);
    }

    return z;
}

/*
 * Initialize matrix in Morton order.
 *
 * A(i,j) = (i+1) + (j+1)/1000
 */
static void initialize_morton(double *a, int N)
{
    for (int i = 0; i < N; ++i)
    {
        for (int j = 0; j < N; ++j)
        {
            size_t idx = morton_index(i, j);

            a[idx] =
                (double)(i + 1)
                + 0.001 * (double)(j + 1);
        }
    }
}

/*
 * Verify:
 *
 * B(i,j) == A(j,i)
 */
static int verify_transpose(const double *b, int N)
{
    for (int i = 0; i < N; ++i)
    {
        for (int j = 0; j < N; ++j)
        {
            size_t idx = morton_index(i, j);

            double expected =
                (double)(j + 1)
                + 0.001 * (double)(i + 1);

            if (fabs(b[idx] - expected) > 1e-12)
            {
                printf("FAIL\n");
                printf("(%d,%d): got %.15f expected %.15f\n",
                       i, j,
                       b[idx],
                       expected);

                return 0;
            }
        }
    }

    return 1;
}

int main(void)
{
    const int tests[] = {
        8, 16, 32, 64, 128, 256, 512
    };

    const int num_tests =
        sizeof(tests) / sizeof(tests[0]);

    for (int t = 0; t < num_tests; ++t)
    {
        int N = tests[t];

        double *a;
        double *b;

        posix_memalign((void**)&a, 64,
                       sizeof(double) * N * N);

        posix_memalign((void**)&b, 64,
                       sizeof(double) * N * N);

        initialize_morton(a, N);

        morton(a, b, N);

        int result = verify_transpose(b, N);

        printf("N=%d : %s\n",
               N,
               result ? "PASS" : "FAIL");

        free(a);
        free(b);

        if (!result)
        {
            return 1;
        }
    }

    printf("\nAll tests passed.\n");

    return 0;
}