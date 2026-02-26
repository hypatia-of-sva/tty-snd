#include "common.h"

int main(int argc, char** argv) {
    assert(argc >= 3);

    waveform_t file_wave;
    int code = read_int_wav_file(argv[1], atoi(argv[2]), &file_wave);
    if(code != 0) goto error;
    
    file_wave.data_length = truncate_power_of_2(file_wave.data_length);
    assert(is_power_of_2(file_wave.data_length));
    
    floating_waveform_t out_wave;
    code = int_wave_to_floating(file_wave, &out_wave);
    if(code != 0) goto error;

	code = write_floating_wave(out_wave, stdout);
	
error:
	destroy_int_wave(&file_wave);
	destroy_floating_wave(&out_wave);
	
	return code;
}
