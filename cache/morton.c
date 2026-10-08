// Generated with GPT-5.6 Sol.
// Prompt used in prompt.txt. GPT-5.6 was able to one-shot this code with no errors.
#include <stddef.h>
#include <immintrin.h>

#include "transposes.h"

/*
 * Morton order used here:
 *
 *     quadrant 0: top-left
 *     quadrant 1: top-right
 *     quadrant 2: bottom-left
 *     quadrant 3: bottom-right
 *
 * Each quadrant occupies a contiguous section of N*N/4 doubles.
 *
 * For a 4x4 matrix, the linear Morton layout is:
 *
 *     a[ 0] = A(0,0)    a[ 1] = A(0,1)
 *     a[ 2] = A(1,0)    a[ 3] = A(1,1)
 *
 *     a[ 4] = A(0,2)    a[ 5] = A(0,3)
 *     a[ 6] = A(1,2)    a[ 7] = A(1,3)
 *
 *     a[ 8] = A(2,0)    a[ 9] = A(2,1)
 *     a[10] = A(3,0)    a[11] = A(3,1)
 *
 *     a[12] = A(2,2)    a[13] = A(2,3)
 *     a[14] = A(3,2)    a[15] = A(3,3)
 */

/*
 * Transpose one Morton-ordered 2x2 block.
 *
 * Input:
 *     [ x0 x1 ]
 *     [ x2 x3 ]
 *
 * Morton order: x0, x1, x2, x3
 *
 * Output:
 *     [ x0 x2 ]
 *     [ x1 x3 ]
 *
 * Morton order: x0, x2, x1, x3
 *
 * _MM_SHUFFLE(3,1,2,0) equals 0xD8 and selects:
 *
 *     destination[0] = source[0]
 *     destination[1] = source[2]
 *     destination[2] = source[1]
 *     destination[3] = source[3]
 */
static inline __m256d
transpose_2x2_morton(__m256d x)
{
    return _mm256_permute4x64_pd(x, _MM_SHUFFLE(3, 1, 2, 0));
}

/*
 * AVX2 4x4 Morton-order transpose.
 *
 * The four 256-bit vectors are the four contiguous Morton-order 2x2
 * quadrants. Each register contains exactly four doubles.
 *
 * Input quadrants:
 *     q0 q1
 *     q2 q3
 *
 * After transposition:
 *     transpose(q0) transpose(q2)
 *     transpose(q1) transpose(q3)
 */
static inline void
morton_kernel_4x4(const double *restrict a,
                  double *restrict b)
{
    __m256d q0 = _mm256_loadu_pd(a +  0);
    __m256d q1 = _mm256_loadu_pd(a +  4);
    __m256d q2 = _mm256_loadu_pd(a +  8);
    __m256d q3 = _mm256_loadu_pd(a + 12);

    q0 = transpose_2x2_morton(q0);
    q1 = transpose_2x2_morton(q1);
    q2 = transpose_2x2_morton(q2);
    q3 = transpose_2x2_morton(q3);

    /*
     * Transposition swaps the off-diagonal quadrants:
     *
     *     input q1 -> output bottom-left
     *     input q2 -> output top-right
     */
    _mm256_storeu_pd(b +  0, q0);
    _mm256_storeu_pd(b +  4, q2);
    _mm256_storeu_pd(b +  8, q1);
    _mm256_storeu_pd(b + 12, q3);
}

/*
 * Optional small base cases make the routine valid for N = 1 or N = 2.
 * The supplied driver begins at N = 4, so these are mainly defensive.
 */
static inline void
morton_kernel_2x2(const double *restrict a,
                  double *restrict b)
{
    b[0] = a[0];
    b[1] = a[2];
    b[2] = a[1];
    b[3] = a[3];
}

static void
morton_recursive(const double *restrict a,
                 double *restrict b,
                 size_t n)
{
    if (n == 4) {
        morton_kernel_4x4(a, b);
        return;
    }

    if (n == 2) {
        morton_kernel_2x2(a, b);
        return;
    }

    if (n == 1) {
        b[0] = a[0];
        return;
    }

    /*
     * In Morton order, each n/2 by n/2 quadrant is contiguous and
     * contains n*n/4 elements.
     */
    const size_t quadrant_size = (n * n) / 4;

    const double *a00 = a;
    const double *a01 = a + quadrant_size;
    const double *a10 = a + 2 * quadrant_size;
    const double *a11 = a + 3 * quadrant_size;

    double *b00 = b;
    double *b01 = b + quadrant_size;
    double *b10 = b + 2 * quadrant_size;
    double *b11 = b + 3 * quadrant_size;

    /*
     * If:
     *
     *     A = [ A00 A01 ]
     *         [ A10 A11 ]
     *
     * then:
     *
     *     transpose(A) = [ transpose(A00) transpose(A10) ]
     *                    [ transpose(A01) transpose(A11) ]
     */
    morton_recursive(a00, b00, n / 2);
    morton_recursive(a10, b01, n / 2);
    morton_recursive(a01, b10, n / 2);
    morton_recursive(a11, b11, n / 2);
}

void
morton(double *a, double *b, int N)
{
    if (a == NULL || b == NULL || N <= 0) {
        return;
    }

    /*
     * This implementation assumes N is a power of two. The assignment
     * driver generates 4, 8, 16, ..., so that condition is satisfied.
     */
    morton_recursive(a, b, (size_t)N);
}