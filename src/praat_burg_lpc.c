#include "common.h"

double VECburg(double* out_coeffs, size_t nr_coeffs, const double  * samples, size_t nr_samples) {

    
    double* a = out_coeffs;
    const double* x = samples;
    const int64_t x_len = nr_samples, a_len = nr_coeffs;
    const int64_t n = x_len, m = a_len;
    
    memset(out_coeffs, 0, a_len*sizeof(double));
    if(x_len <= 2) {
        a[0] = -1.0;
        return ( x_len == 2 ? 0.5 * (x [0] * x [0] + x [1] * x [1]) : x [0] * x [0] );
    }
    
    double* b1 = calloc(x_len, sizeof(double));
    double* b2 = calloc(x_len, sizeof(double));
    double* aa = calloc(a_len, sizeof(double));
    
    
	// (3)
    
    long double p = 0.0;
    for(int64_t j = 0; j < x_len; j++) {
        p += x[j]*x[j];
    }
    long double xms = p / n;
    
	if (xms <= 0.0)
		return xms;	// warning empty
    
	// (9)
    
    b1[0] = x[0];
    b2 [x_len - 2] = x [x_len-1];
    
    for(int64_t j = 1; j < x_len-1; j++) {
        b1 [j] = b2 [j - 1] = x [j];
    }
    
    
	for (int64_t i = 0; i < a_len; i ++) {
		// (7)

		long double num = 0.0, denum = 0.0;
		for (int64_t j = 0; j < x_len - (i+1); j ++) {
			num += b1 [j] * b2 [j];
			denum += b1 [j] * b1 [j] + b2 [j] * b2 [j];
		}

		if (denum <= 0.0)
			return 0.0;	// warning ill-conditioned

		a [i] = 2.0 * num / denum;

		// (10)

		xms *= 1.0 - a [i] * a [i];

		// (5)

		for (int64_t j = 0; j < i; j ++)
			a [j] = aa [j] - a [i] * aa [i - j  -1];

		if (i+1 < m) {

			// (8) Watch out: i -> i+1

			for (int64_t j = 0; j < i+1; j ++)
				aa [j] = a [j];
			for (int64_t j = 0; j < x_len - (i+1) - 1; j ++) {
				b1 [j] -= aa [i] * b2 [j];
				b2 [j] = b2 [j + 1] - aa [i] * b1 [j + 1];
			}
		}
	}
	return xms;
}
