/* polynomial root finding algorithm, from GLS 2.8 (poly/zsolve_init.c and poly/zsolve.c)
 * extracted and minified by Hypatia of Sva, 2025
 *
 * Original Copyright notice:
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2004, 2007 Brian Gough
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <stddef.h>
#include <math.h>

void die(const char* str);

#define MY_DBL_EPSILON        2.2204460492503131e-16
/* defines inlined row-major matrix access */
#define MAT(i,j) working_matrix[(i)*len+j]

/** finds the complex roots of  = 0
 * @param polynomial_coefficients  must point to an array of at least nr_of_terms coefficient values, interpreted as the real coefficients of the polynomial, and not changed
 * @param polynomial_roots         must point to an array of at least 2*(nr_of_terms-1) doubles that can be written to, will contain the real and complex values of the roots in alternating order
 * @param working_matrix           must point to an array of at least (nr_of_terms - 1)*(nr_of_terms - 1) doubles that can be written to, will include temporary working order, can be discarded
*/
void poly_complex_solve(const double *polynomial_coefficients, size_t nr_of_terms, double* polynomial_roots, double* working_matrix) {
    if (nr_of_terms == 0) {
        die("number of terms must be a positive integer"); /* EINVAL */
    }
    if (nr_of_terms == 1) {
        die("cannot solve for only one term"); /* EINVAL */
    }
    if (polynomial_coefficients[nr_of_terms - 1] == 0) {
        die("leading term of polynomial must be non-zero"); /* EINVAL */
    }

    size_t len = nr_of_terms - 1;

    /* setup the companion working matrix: */
    for (int i = 0; i < len; i++)
        for (int j = 0; j < len; j++)
            MAT(i, j) = 0.0;
    for (int i = 1; i < len; i++)
        MAT(i, i - 1) = 1.0;
    for (int i = 0; i < len; i++)
        MAT(i, len - 1) = -polynomial_coefficients[i] / polynomial_coefficients[nr_of_terms-1];

    /* balance the companion working matrix: */
    int not_converged = 1;
    while (not_converged) {
        not_converged = 0;
        for (int i = 0; i < len; i++) {
            double row_norm = 0;
            double col_norm = 0;
            /* column norm, excluding the diagonal */
            if (i != len - 1) {
                col_norm = fabs (MAT(i + 1, i));
            } else {
                col_norm = 0;
                for (int j = 0; j < len - 1; j++) {
                    col_norm += fabs (MAT(j, len - 1));
                }
            }
            /* row norm, excluding the diagonal */
            if (i == 0) {
                row_norm = fabs (MAT(0, len - 1));
            } else if (i == len - 1) {
                row_norm = fabs (MAT(i, i - 1));
            } else {
                row_norm = (fabs (MAT(i, i - 1))
                          + fabs (MAT(i, len - 1)));
            }

            if (col_norm == 0 || row_norm == 0) {
                continue;
            }

            double f = 1;
            double s = col_norm + row_norm;

            while (col_norm < row_norm / 2) {
                f *= 2;
                col_norm *= 4;
            }
            while (col_norm > row_norm * 2) {
                f /= 2;
                col_norm /= 4;
            }

            if ((row_norm + col_norm) < 0.95 * s * f) {
                not_converged = 1;
                if (i == 0) {
                    MAT(0, len - 1) /= f;
                } else {
                    MAT(i, i - 1) /= f;
                    MAT(i, len - 1) /= f;
                }

                if (i == len - 1) {
                    for (int j = 0; j < len; j++) {
                        MAT(j, i) *= f;
                    }
                } else {
                    MAT(i + 1, i) *= f;
                }
            }
        }
    }


    /* perform the QR algorithm on the companion working matrix: */
    double t = 0.0;
    size_t iterations = 0;
    size_t n = len;
/*next_iteration: */
    while (1) {
        double x, y, w;
        size_t e,m;

        for (e = n; e >= 2; e--) {
            double a1 = fabs (MAT(e-1,e-2));
            double a2 = fabs (MAT(e-2,e-2));
            double a3 = fabs (MAT(e-1,e-1));
            if (a1 <= MY_DBL_EPSILON * (a2 + a3))
                break;
        }
        x = MAT(n-1, n-1);
        if (e == n) {
            /* one real root */
            polynomial_roots[2*(n-1)] = x + t;
            polynomial_roots[2*(n-1)+1] = 0;
            n--;
            if (n == 0)
                return; /* SUCCESS! */
            iterations = 0;
            continue; /*goto next_iteration;*/
        }
        y = MAT(n-2,n-2);
        w = MAT(n-2,n-1) * MAT(n-1,n-2);
        if (e == n - 1) {
            double p = (y - x) / 2;
            double q = p * p + w;
            y = sqrt (fabs (q));
            if (q > 0) {
                /* two real roots */
                if (p < 0)
                    y = -y;
                y += p;
                polynomial_roots[2*(n-1)] = (x+t) - w / y;
                polynomial_roots[2*(n-1)+1] = 0;
                polynomial_roots[2*(n-2)] = (x+t) + y;
                polynomial_roots[2*(n-2)+1] = 0;
            } else {
                polynomial_roots[2*(n-1)] = (x+t) + p;
                polynomial_roots[2*(n-1)+1] = -y;
                polynomial_roots[2*(n-2)] = (x+t) + p;
                polynomial_roots[2*(n-2)+1] = y;
            }
            n -= 2;
            if (n == 0)
                return; /* SUCCESS! */
            iterations = 0;
            continue; /*goto next_iteration;*/
        }
        /* No more roots found yet, do another iteration */
        if (iterations == 120) { /* increased from 30 to 120 */
            /* too many iterations - give up! */
            die("root solving qr method failed to converge"); /* EFAILED */
        }
        if (iterations % 10 == 0 && iterations > 0) {
            /* use an exceptional shift */
            t += x;
            for (int i = 1; i <= n; i++) {
                MAT(i-1, i-1) -= x;
            }
            double s = fabs (MAT(n-1, n-2)) + fabs (MAT(n - 2, n - 3));
            y = 0.75 * s;
            x = y;
            w = -0.4375 * s * s;
        }
        iterations++;

        double p = 0, q = 0, r = 0;
        /* FIXME: if p,q,r, are not set to zero then the compiler complains
            that they ``might be used uninitialized in this
            function''. Looking at the code this does seem possible, so this
            should be checked. */
        for (m = n - 2; m >= e; m--) {
            double a1, a2, a3;

            double z = MAT(m-1, m-1);
            p = MAT(m-1, m) + ((x-z)*(y-z) - w) / MAT(m, m-1);
            q = MAT(m, m) - x - y + z;
            r = MAT(m + 1, m);
            double s = fabs (p) + fabs (q) + fabs (r);
            p /= s;
            q /= s;
            r /= s;

            if (m == e)
                break;

            a1 = fabs (MAT(m-1,m-2));
            a2 = fabs (MAT(m-2,m-2));
            a3 = fabs (MAT(m,m));

            if (a1 * (fabs (q) + fabs (r)) <= MY_DBL_EPSILON * fabs (p) * (a2 + a3))
                break;
        }

        for (int i = m + 2; i <= n; i++) {
            MAT(i-1, i-3) = 0;
        }
        for (int i = m + 3; i <= n; i++) {
            MAT(i-1, i-4) = 0;
        }

        /* double QR step */
        double s = sqrt (p * p + q * q + r * r);
        if (p < 0) s = -s;
        if (e != m) {
            MAT (m-1, m-2) *= -1;
        }
        /* do row modifications */
        for (int j = m; j <= n; j++) {
            double p2 = MAT(m-1, j-1) + (q/(p+s)) * MAT(m, j-1) + (r/(p+s)) * MAT(m+1, j-1);
            MAT(m+1, j-1) -= p2 * (r/s);
            MAT(m, j-1) -= p2 * (q/s);
            MAT(m-1, j-1) -= p2 * ((p+s)/s);
        }
        /* do column modifications */
        for (int i = e; i <= ((m + 3 < n) ? (m + 3) : n); i++) {
            double p2 = ((p+s)/s) * MAT(i-1, m-1) + (q/s) * MAT(i-1, m) + (r/s) * MAT(i-1, m+1);
            MAT(i-1, m + 1) -= p2 * (r/(p+s));
            MAT(i-1, m) -= p2 * (q/(p+s));
            MAT(i-1, m-1) -= p2;
        }

        for (int k = m+1; k <= n - 1; k++) {
            int notlast = (k != n - 1);
            double p = MAT(k-1, k-2);
            double q = MAT(k, k - 2);
            double r = notlast ? MAT(k + 1, k - 2) : 0.0;

            double x = fabs (p) + fabs (q) + fabs (r);

            if (x == 0)
                continue; /* inner loop! */           /* FIXME????? */

            p /= x; q /= x; r /= x;
            double s = sqrt (p * p + q * q + r * r);
            if (p < 0) s = -s;

            MAT (k-1, k-2) = -s * x;
            /* do row modifications */
            for (int j = k; j <= n; j++) {
                double p2 = MAT(k-1, j-1) + (q/(p+s)) * MAT(k, j-1);
                if (notlast) {
                    p2 += (r/(p+s)) * MAT(k+1, j-1);
                    MAT(k+1, j-1) -= p2 * (r/s);
                }
                MAT(k, j-1) -= p2 * (q/s);
                MAT(k-1, j-1) -= p2 * ((p+s)/s);
            }
            /* do column modifications */
            for (int i = e; i <= ((k + 3 < n) ? (k + 3) : n); i++) {
                double p2 = ((p+s)/s) * MAT(i-1, k-1) + (q/s) * MAT(i-1, k);
                if (notlast) {
                    p2 += (r/s) * MAT(i-1, k + 1);
                    MAT(i-1, k + 1) -= p2 * (r/(p+s));
                }
                MAT(i-1, k) -= p2 * (q/(p+s));
                MAT(i-1, k-1) -= p2;
            }
        }
    }
}
