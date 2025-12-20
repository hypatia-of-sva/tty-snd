#include "common.h"
/*
#define FORMANTS_IMPLEMENTATION
#include "libformants.h"
*/

int main(int argc, char** argv) {
    assert(argc > 3);
    int order = atoi(argv[1]);
    int maxbw = atoi(argv[2]);
    int minformant = atoi(argv[3]);

    simple_wav_t float_form = read_simple_wav(stdin);



	double* formants = calloc(order, sizeof(double));
	double* bws = calloc(order, sizeof(double));
	bool* is_selected = calloc(order, sizeof(bool));
	size_t len = float_form.nr_sample_points / 2;
	
	double* data = calloc(len, sizeof(double));
	for(int i = 0; i < len; i++) {
		data[i] = float_form.samples[2*i];
	}

	r_find_formants(data, len, float_form.frequency_in_hz, order, maxbw, minformant, formants, bws, is_selected);
	
	printf("formant values by R algorithm (order = %i, maxbw = %i, minformant = %i): \n", order, maxbw, minformant);
	for(int i = 0; i < order; i++) {
		printf("[%i]: %f +- %f Hz, selected: %i\n", i, formants[i], bws[i], is_selected[i]);
	}
	
	return 0;

}
