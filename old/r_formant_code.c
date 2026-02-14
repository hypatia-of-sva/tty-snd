#include "common.h"

/*
    Based on the r-package phonTools
    Copyright (c) 2015 Santiago Barreda
    All rights reserved.

*/


/*
 * formants, bws, is_selected needs to be at least order, and sound at least len long
 *  */

void r_find_formants(double* sound, size_t len, double frequency, int order, int maxbw, int minformant, double* formants, double* bws, bool* is_selected) {
	int i;
	
    if(order == 0)
        order = round(frequency/1000.0)+3;
    if(maxbw == 0)
        maxbw = 600;
    if(minformant == 0)
        minformant = 200;

    assert(sound != NULL && len > 1);




    int preemph_cutoff = 50;
    double preemph_coeff = -exp (-2.0 * M_PI * preemph_cutoff / frequency);
    // filter
    double curr_value, old_value = sound[0];
    double mean = sound[0];
    for(i = 1; i < len; i++) {
        curr_value = sound[i];
        if (isnan(curr_value) || isnan(old_value)) {
            sound[i] = NAN;
        } else {
            sound[i] = curr_value + preemph_coeff*old_value;
        }
        /* mean calculation folded in so we don't have to traverse again*/
        mean += sound[i];
    }
    mean /= len;

    for(i = 0; i < len; i++) {
        sound[i] -= mean;
        // window func
        sound[i] *= 0.5 * (1 - cos ((2*M_PI*i)/(len-1)));
    }

    double* r = calloc(order+1, sizeof(double));
    for(int k = 0; k < order+1; k++) {
        r[k] = 0.0;
        for(i = 0; i < len-k; i++) {
            r[k] += sound[i]*sound[i+k];
        }
    }


    matrix_t w = create_matrix(order, order);
    for(int i = 0; i < order; i++) {
        for(int j = 0; j < order; j++) {
            /* diagonals r[0], "Nebendiagonalen" r[1], etc.
                example:
                say order = 5

                r[0]    r[1]    r[2]    r[3]    r[4]
                r[1]    r[0]    r[1]    r[2]    r[3]
                r[2]    r[1]    r[0]    r[1]    r[2]
                r[3]    r[2]    r[1]    r[0]    r[1]
                r[4]    r[3]    r[2]    r[1]    r[0]

                */
            set(w, i,j, r[abs(i-j)]);
        }
    }

    matrix_t w_inv = invert_matrix(w);

    matrix_t b = create_matrix(order, 1);
    for(int i = 0; i < order; i++) {
        set(b, i, 0, -r[i+1]);
    }

    matrix_t m_coeffs = matrix_multiply(b, w_inv);

    double* rev_coeffs = calloc(order+1, sizeof(double));
    for(int i = 0; i < order; i++) {
        rev_coeffs[order-i] = get(m_coeffs,(order-1)-i,0);
    }
    rev_coeffs[0] = 1.0;
    
    double* roots = calloc(2*order, sizeof(double));
    double* working_mat = calloc(order*order, sizeof(double));
    poly_complex_solve(rev_coeffs, order+1, roots, working_mat);
    free(working_mat);
    free(rev_coeffs);
    
    
    for(int i = 0; i < order; i++) {
		formants[i] = atan2(roots[2*i+1], roots[2*i]) * (frequency/2*M_PI);
		formants[i] = round(formants[i]*100.0)/100.0; /* to two decimal digits */
		bws[i] = -(frequency/M_PI)*log(roots[2*i]*roots[2*i] + roots[2*i+1]*roots[2*i+1]);
		is_selected[i] = (bws[i] < maxbw && formants[i] > minformant && formants[i] < frequency/2);
	}
}
