#include "common.h"

int main(int argc, char** argv) {
    assert(argc >= 2);
    
	floating_waveform_t in_wave;
	assert(read_floating_wave(&in_wave, stdin) == 0);
	
    waveform_t file_wave;
    int code = floating_wave_to_int(in_wave, &file_wave);
    if(code != 0) goto error;

	FILE* fp = fopen(argv[1], "wb");
	code = write_int_wav_file(fp, file_wave);
    if(code != 0) goto error;
    
error:
	destroy_floating_wave(&in_wave);
	destroy_int_wave(&file_wave);
	fclose(fp);
	
	return code;
}
