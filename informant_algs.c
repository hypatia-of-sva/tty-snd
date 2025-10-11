#include "common.h"


/* curtesy of the in-formant project: https://github.com/in-formant/in-formant */


double* autocorr_solve(const double *data, int length, int lpcOrder, double *pGain, size_t* out_nr_formants)
{
    double gain;
    int i, j;

    const int n = length;
    const int m = lpcOrder;


    double* r = calloc(m+2, sizeof(double));
    double* a = calloc(m+2, sizeof(double));
    double* rc = calloc(m+2, sizeof(double));

    j = m + 1;
    while (j--) {
        double d = 0.0;
        for (i = j; i < n; ++i)
            d += data[i] * data[i - j];
        r[j + 1] = d;
    }
    if (r[1] == 0.0) {
        i = 1;
        gain = 1e-10;
        goto end;
    }

    a[1] = 1.0;
    a[2] = rc[1] = -r[2] / r[1];
    gain = r[1] + r[2] * rc[1];

    for (i = 2; i <= m; ++i) {
        double s = 0.0;
        for (j = 1; j <= i; ++j)
            s += r[i - j + 2] * a[j];
        rc[i] = -s / gain;
        for (j = 2; j <= i / 2 + 1; ++j) {
            const double at = a[j] + rc[i] * a[i - j + 2];
            a[i - j + 2] += rc[i] * a[j];
            a[j] = at;
        }
        a[i + 1] = rc[i];
        gain += rc[i] * s;
        if (gain <= 0.0)
            goto end;
    }

end:
    --i;

    if (pGain != nullptr)
        *pGain = gain;


    out_nr_formants[0] = i;
    double* lpc = calloc(i, sizeof(double));

    for (j = 1; j <= i; ++j)
       lpc[j - 1] = a[j + 1];



    free(r);
    free(a);
    free(rc);



    return lpc;
}




double* Covar_solve(const double *data, int length, int lpcOrder, double *pGain, size_t* out_nr_formants)

{

    double gain;
    int i, j, k;

    const int n = length;
    const int m = lpcOrder;

    double* b = calloc(1 + (m * (m + 1) / 2), sizeof(double));
    double* grc = calloc(1 + (m), sizeof(double));
    double* beta = calloc(1 + (m), sizeof(double));
    double* a = calloc(1 + (m + 1), sizeof(double));
    double* cc = calloc(1 + (m + 1), sizeof(double));


    // Simulating index-1-based array.
    const double *x = &data[-1];

    gain = 0.0;
    for (i = m + 1; i <= n; ++i) {
        gain += x[i] * x[i];
        cc[1] += x[i] * x[i - 1];
        cc[2] += x[i - 1] * x[i - 1];
    }
    if (gain == 0.0) {
        i = 1;
        gain = 1e-10;
        goto end;
    }

    b[1] = 1.0;
    beta[1] = cc[2];
    a[1] = 1.0;
    a[2] = grc[1] = -cc[1] / cc[2];
    gain += grc[1] * cc[1];

    for (i = 2; i <= m; ++i) {
        double s = 0.0;
        for (j = 1; j <= i; ++j)
            cc[i - j + 2] = cc[i - j + 1]
                            + x[m - i + 1] * x[m - i + j]
                            - x[n - i + 1] * x[n - i + j];

        cc[1] = 0.0;
        for (j = m + 1; j <= n; ++j)
            cc[1] += x[j - i] * x[j];

        b[i * (i + 1) / 2] = 1.0;
        for (j = 1; j <= i - 1; ++j) {
            double gam = 0.0;
            if (beta[j] < 0.0)
                goto end;
            else if (beta[j] == 0.0)
                continue;

            for (k = 1; k <= j; ++k)
                gam += cc[k + 1] * b[j * (j - 1) / 2 + k];

            gam /= beta[j];
            for (k = 1; k <= j; ++k)
                b[i * (i - 1) / 2 + k] -= gam * b[j * (j - 1) / 2 + k];
        }

        beta[i] = 0.0;
        for (j = 1; j <= i; ++j)
            beta[i] += cc[j + 1] * b[i * (i - 1) / 2 + j];
        if (beta[i] <= 0.0)
            goto end;

        for (j = 1; j <= i; ++j)
            s += cc[j] * a[j];
        grc[i] = -s / beta[i];

        for (j = 2; j <= i; ++j)
            a[j] += grc[i] * b[i * (i - 1) / 2 + j - 1];
        a[i + 1] = grc[i];
        s = grc[i] * grc[i] * beta[i];
        gain -= s;
        if (gain <= 0.0)
            goto end;
    }

end:
    --i;

    if (pGain != nullptr)
        *pGain = gain;


    out_nr_formants[0] = i;
    double* lpc = calloc(i, sizeof(double));

    for (j = 1; j <= i; ++j)
       lpc[j - 1] = a[j + 1];



    free(b);
    free(grc);
    free(beta);
    free(a);
    free(cc);


    return lpc;
}





static double vecBurgBuffered(
        double *lpc,
        const int m,
        const double *data,
        const int n)
{
    int i, j;


    double* b1 = calloc(1+n, sizeof(double));
    double* b2 = calloc(1+n, sizeof(double));
    double* aa = calloc(1+m, sizeof(double));

    // Simulating index-1-based array.
    double *a = &lpc[-1];
    const double *x = &data[-1];

    double p = 0.0;
    for (j = 1; j <= n; ++j)
        p += x[j] * x[j];

    double xms = p / n;
    if (xms <= 0.0) {
        free(b1); free(b2); free(aa);
        return xms;
    }

    b1[1] = x[1];
    b2[n - 1] = x[n];
    for (j = 2; j <= n - 1; ++j)
        b1[j] = b2[j - 1] = x[j];

    for (i = 1; i <= m; ++i) {
        double num = 0.0, denum = 0.0;
        for (j = 1; j <= n - i; ++j) {
            num += b1[j] * b2[j];
            denum += b1[j] * b1[j] + b2[j] * b2[j];
        }

        if (denum <= 0.0) {
            free(b1); free(b2); free(aa);
            return 0.0;
        }

        a[i] = 2.0 * num / denum;

        xms *= 1.0 - a[i] * a[i];

        for (j = 1; j <= i - 1; ++j)
            a[j] = aa[j] - a[i] * aa[i - j];

        if (i < m) {
            for (j = 1; j <= i; ++j)
                aa[j] = a[j];
            for (j = 1; j <= n - i - 1; ++j) {
                b1[j] -= aa[i] * b2[j];
                b2[j] = b2[j + 1] - aa[i] * b1[j + 1];
            }
        }
    }

    free(b1); free(b2); free(aa);
    return xms;
}

double* Burg_solve(const double *x, int length, int lpcOrder, double *pGain, size_t* out_nr_formants)
{
    const int n = length;
    const int m = lpcOrder;

    double* lpc = calloc(m, sizeof(double));

    double gain = vecBurgBuffered(lpc, m, x, n);


    if (gain <= 0.0) {
        out_nr_formants[0] = 0;
        gain = 1e-10;
    } else {
        out_nr_formants[0] = m;
    }

    gain *= n;

    for (int i = 0; i < out_nr_formants[0]; i++) {
        lpc[i] *= -1;
    }

    if (pGain != NULL)
        *pGain = gain;

    return lpc;
}



