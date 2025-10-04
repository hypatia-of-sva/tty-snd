#include "common.h"




int main(int argc, char** argv) {
    assert(argc >= 2);
    int order = atoi(argv[1]);

    simple_wav_t float_form = read_simple_wav(stdin);

    simple_wav_t out_form = {0};
    out_form.frequency_in_hz = float_form.frequency_in_hz;
    out_form.nr_sample_points = float_form.nr_sample_points/2;



    double* data = calloc(sizeof(double),out_form.nr_sample_points);

    /*printf("data = [");*/
    for(int i = 0; i < out_form.nr_sample_points; i++) {
        double value = float_form.samples[2*i]; /*sqrt(float_form.samples[2*i]*float_form.samples[2*i]
                        + float_form.samples[2*i+1]*float_form.samples[2*i+1]) */;
        data[i] = value;
        /*printf("%f,", value);
        if((i % 100) == 0) printf("\n"); */
    }
    /*printf("]\n");*/

    printf("lpc = [");
    double* lpc_params = lpc_coefficients_rosa_double(data, out_form.nr_sample_points, order);
    for(int i = 0; i < order+1; i++) {
        printf("%f,", lpc_params[i]);
    }
    printf("]\n");


    double* coeffs = calloc(order+1, sizeof(double));
    for(int i = 0; i < order+1; i++) {
        coeffs[i] = lpc_params[order-i];
    }

    double* roots = calloc(order*2, sizeof(double));
    double* mat = calloc(order*order, sizeof(double));
    poly_complex_solve(coeffs, order+1, roots, mat);
    free(mat);

    formant_t* formants = calloc(order, sizeof(formant_t));
    printf("roots = [");
    for(int i = 0; i < order; i++) {
        printf("%f+i%f,", roots[2*i], roots[2*i+1]);
        double abs_val_squared = (roots[2*i]*roots[2*i] + roots[2*i+1]*roots[2*i+1]);
        formants[i].bw = -0.25 * (float_form.frequency_in_hz / (2*M_PI)) * log(abs_val_squared);
        formants[i].freq = atan2(roots[2*i], roots[2*i+1]) * (float_form.frequency_in_hz / (2*M_PI));
        formants[i].stable_and_keep = (abs_val_squared < 1) && (formants[i].freq > 0);
    }
    printf("]\n");

    qsort(formants, order, sizeof(formant_t), formant_by_freq_cmp_qsort);

    for(int i = 0; i < order; i++) {
        if(formants[i].stable_and_keep)
            printf("freq = %fHz +- %f\n", formants[i].freq, formants[i].bw);
    }


    return 0;
}
