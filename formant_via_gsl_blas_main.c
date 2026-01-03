#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_poly.h>
#include "common.h"



float calculate_ar_factor(double* data, size_t len, int k) {
    float res = 0.0f;
    for(int i = 0; i < len - k; i++) {
        res += data[i]*data[i+k];
    }
    return res;
}


gsl_vector* lpc_params(double* data, size_t len, int nr_formants) {
    gsl_matrix *A = gsl_matrix_alloc(nr_formants, nr_formants);
    gsl_vector *b = gsl_vector_alloc(nr_formants);
    for(int i = 0; i < nr_formants; i++) {
        float factor = calculate_ar_factor(data, len, i+1);
        gsl_vector_set(b, i, factor);
        for(int k = 0; k < nr_formants-(i+1); k++) {
            gsl_matrix_set(A, (i+1)+k, k, factor);
            gsl_matrix_set(A, k, (i+1)+k, factor);
        }
    }
    float base_factor = calculate_ar_factor(data, len, 0);
    for(int i = 0; i < nr_formants; i++) {
        gsl_matrix_set(A, i, i, base_factor);
    }
    
    gsl_matrix *T = gsl_matrix_alloc(nr_formants, nr_formants);
    gsl_vector *x = gsl_vector_alloc(nr_formants);
    gsl_linalg_QR_decomp_r(A, T);
    gsl_linalg_QR_solve_r(A, T, b, x);
    
    gsl_matrix_free(A); gsl_matrix_free(T); gsl_vector_free(b);
    return x;
}


double* lpc_roots(double* data, size_t len, int nr_formants) {
	gsl_vector* lpc_coeffs = lpc_params(data, len, nr_formants);
	double* poly_coeffs = calloc(nr_formants+1, sizeof(double));
    poly_coeffs[nr_formants] = 1.0;
    for(int i = 0; i < nr_formants; i++) {
        poly_coeffs[nr_formants-i-1] = - gsl_vector_get(lpc_coeffs, i);
    }
    double* poly_roots = calloc(nr_formants*2, sizeof(double));
    
    gsl_poly_complex_workspace * w = gsl_poly_complex_workspace_alloc (nr_formants+1);
	gsl_poly_complex_solve (poly_coeffs, nr_formants+1, w, poly_roots);
	gsl_poly_complex_workspace_free (w);
	free(poly_coeffs);

	return poly_roots;
}


void print_formants(double* data, size_t len, int nr_formants, float sample_freq) {
	double* roots = lpc_roots(data, len, nr_formants);
	
	for(int i = 0; i < nr_formants; i++) {
		double real = roots[2*i], complex = roots[2*i+1];
		double freq = (sample_freq / 2*M_PI)*atan2(complex, real);
		double bw = (-sample_freq / M_PI)*log(sqrt(real*real + complex*complex));
		
		printf("formant %i: %f Hz +- %f\n", i, freq, bw);
	}
}



int main(int argc, char** argv) {
	assert(argc >= 2);
	int nr_formants = atoi(argv[1]);
	
    simple_wav_t float_form = read_simple_wav(stdin);
    
    double* data = calloc(sizeof(double), float_form.nr_sample_points);
    for(int i = 0; i < float_form.nr_sample_points; i++) {
		data[i] = (double) float_form.samples[i];
	}

	print_formants(data, float_form.nr_sample_points, nr_formants,
		float_form.frequency_in_hz);

	return 0;
}
