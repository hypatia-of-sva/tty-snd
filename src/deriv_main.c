#include "common.h"
#include <gsl/gsl_deriv.h>


double my_f_of_wave(double x, void* p) {
	floating_waveform_t* wave = (floating_waveform_t*) p;
	if(x < 0 || x >= wave[0].data_length) {
		return 0.0;
	} else {
		int base_index = (int) floor(x);
		double frac_part = x - floor(x);
		double y0 = wave[0].amplitude_data[base_index];
		double y1 = wave[0].amplitude_data[base_index+1];
		return y0 + (y0-y1)*frac_part;
	}
}


double* deriv(floating_waveform_t wave) {
	double* return_array = calloc(wave.data_length, sizeof(double));
	assert(return_array != NULL);
	
	gsl_function F;
	F.function = &my_f_of_wave;
	F.params = &wave;
	
	for(size_t i = 0; i < wave.data_length; i++) {
		double abserr;
		gsl_deriv_central(&F, (double) i, 1.5, &(return_array[i]), &abserr);
	}
	
	return return_array;
}


int main(int argc, char** argv) {
	floating_waveform_t in_wave;
	assert(read_floating_wave(&in_wave, stdin) == 0);
    
    floating_waveform_t out_wave;
    out_wave.sample_frequency_Hz = in_wave.sample_frequency_Hz;
    out_wave.data_length = in_wave.data_length;
    
    
    out_wave.amplitude_data = deriv(in_wave);

	int code = write_floating_wave(out_wave, stdout);
	
	destroy_floating_wave(&in_wave);
	destroy_floating_wave(&out_wave);
	
	return code;
}


