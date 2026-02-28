#include "common.h"

int main(int argc, char** argv) {
	floating_waveform_t in_wave;
	assert(read_floating_wave(&in_wave, stdin) == 0);
    
    floating_waveform_t out_wave;
    out_wave.sample_frequency_Hz = in_wave.sample_frequency_Hz;
    out_wave.data_length = in_wave.data_length/2;
    out_wave.amplitude_data = calloc(in_wave.data_length/2, sizeof(double));
    for(int i = 0; i < in_wave.data_length/2; i++) {
		out_wave.amplitude_data[i] = in_wave.amplitude_data[2*i];
	}

	int code = write_floating_wave(out_wave, stdout);
	
	destroy_floating_wave(&in_wave);
	destroy_floating_wave(&out_wave);
	
	return code;
}

