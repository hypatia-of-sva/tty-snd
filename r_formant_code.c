#include "common.h"

/*
    Based on the r-package phonTools
    Copyright (c) 2015 Santiago Barreda
    All rights reserved.

*/



double* find_formants(double* sound, size_t len, double frequency, int order, int maxbw, int minformant) {
    if(order == 0)
        order = round(frequency/1000.0)+3;
    if(maxbw == 0)
        maxbw = 600;
    if(minformant == 0)
        minformant = 200;

    assert(sound != NULL && len > 1);




    int preemph_cutoff = 50;
    double preemph_coeff = -exp (-2.0 * M_PI * cutoff / frequency);
    // filter
    double curr_value, old_value = sound[0];
    double mean = sound[0];
    for(i = 1; i < len; i++) {
        curr_value = sound[i];
        if (isnan(curr_value) || isnan(old_value)) {
            sound[i] = NAN;
        } else {
            sound[i] = curr_value + coeff*old_value;
        }
        /* mean calculation folded in so we don't have to traverse again*/
        mean += sound[i];
    }
    mean /= len;

    for(i = 0; i < len; i++) {
        sound[i] -= mean;
        // window func
        sound[i] *= 0.5 * (1 - cos ((2*pi*i)/(len-1)));
    }

    double* r = calloc(order+1, sizeof(double));
    for(int k = 0; k < order+1; k++) {
        r[k] = 0.0;
        for(i = 0; i < len-k; i++) {
            r[i] += sound[i]*sound[i+k];
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

    matrix_t coeffs = create_matrix(order+1, 1);
    for(int i = 0; i < order; i++) {
        set(coeffs, i, 0, get(m_coeffs(m_coeffs,(order-1)-i,0)));
    }
    set(coeffs, order, 0, 1);

    /*
     *
     * still to analyze:
  roots = polyroot (rev(coeffs))
  angs = atan2 (Im(roots), Re(roots))
  formants = round (angs * (frequency(sound)/(2*pi)), 2)

  formants = sort(formants)

  bws = -(frequency(sound)/pi) * log (abs(roots[nums]))
  touse = (bws < maxbw & formants > minformant & formants < frequency(sound)/2)
  out = data.frame (formant = formants[touse], bandwidth = bws[touse])
     *
    */

}
