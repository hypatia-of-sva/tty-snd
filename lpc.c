#include "common.h"

float* lpc_coefficients_rosa(float* data, size_t len, int order) {
    assert(order > 0);

    float* ar_coeffs = calloc(order+1, sizeof(float));
    ar_coeffs[0] = 1;
    float* ar_coeffs_old = calloc(order+1, sizeof(float));

    float den = 0.0f;
    for(int i = 0; i < len-1; i++) {
        assert(isfinite(data[i]) && isfinite(data[i+1]));
        den += data[i]*data[i] + data[i+1]*data[i+1];
    }

    float bwd0 = data[0];

    for(int i = 0; i < order; i++) {
        float rc0 = 0.0f;
        rc0 += bwd0*data[i+1];
        for(int k = 1; k < len-i; k++) {
            rc0 += data[k]*data[i+1+k];
        }
        rc0 *= (-2) / (den + FLT_EPSILON);

        bwd0 += rc0*data[i+1];
        den *= (1.0 - rc0*rc0);
        den -= data[(len-1)-(i+1)]*data[(len-1)-(i+1)];
        den -= (data[i+1] + rc0*bwd0)*(data[i+1] + rc0*bwd0);

        memmove(ar_coeffs_old, ar_coeffs, (order+1)* sizeof(float));
        for(int j = 1; j < i+2; j++) {
            ar_coeffs[j] = ar_coeffs_old[j] + rc0*ar_coeffs_old[i - j + 1];
        }
    }

    free(ar_coeffs_old);
    return ar_coeffs;
}










double* lpc_coefficients_rosa_double(double* data, size_t len, int order) {
    assert(order > 0);

    double* ar_coeffs = calloc(order+1, sizeof(double));
    ar_coeffs[0] = 1;
    double* ar_coeffs_old = calloc(order+1, sizeof(double));

    double den = 0.0f;
    for(int i = 0; i < len-1; i++) {
        if(!(isfinite(data[i]) && isfinite(data[i+1]))) {
            printf("%i failed", i);
        }
        assert(isfinite(data[i]) && isfinite(data[i+1]));
        den += data[i]*data[i] + data[i+1]*data[i+1];
    }

    double bwd0 = data[0];

    for(int i = 0; i < order; i++) {
        double rc0 = 0.0f;
        rc0 += bwd0*data[i+1];
        for(int k = 1; k < len-i; k++) {
            rc0 += data[k]*data[i+1+k];
        }
        rc0 *= (-2) / (den + DBL_EPSILON);

        bwd0 += rc0*data[i+1];
        den *= (1.0 - rc0*rc0);
        den -= data[(len-1)-(i+1)]*data[(len-1)-(i+1)];
        den -= (data[i+1] + rc0*bwd0)*(data[i+1] + rc0*bwd0);

        memmove(ar_coeffs_old, ar_coeffs, (order+1)* sizeof(double));
        for(int j = 1; j < i+2; j++) {
            ar_coeffs[j] = ar_coeffs_old[j] + rc0*ar_coeffs_old[i - j + 1];
        }
    }

    free(ar_coeffs_old);
    return ar_coeffs;
}
