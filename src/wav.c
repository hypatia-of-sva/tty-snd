#include "common.h"

void destroy_waveform(waveform_t* wave) {
	if(wave[0].amplitude_data != NULL)
		free(wave[0].amplitude_data);
}





/*
	Reads a 16 bit PCM Wave file bytewise; kills the program if it doesn't work (gory!)
	For documentation see the specs and overview at
	https://www.mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/WAVE.html
*/
int read_int_wav_file(FILE* fp, int chosen_channel, waveform_t* out_waveform) {
	size_t remaining, block_remaining;
	char block_label[4];
	int found_format_block = 0;
	int available_channels, bytes_per_sample;
    int i, ch;
	
	if(fp == NULL) {
		log_err("File Pointer is a NULL pointer!");
		return -1;
	}
	if(out_waveform == NULL) {
		log_err("Waveform param is a NULL pointer!");
		return -1;
	}

	if(fgetc(fp) != 'R' || fgetc(fp) != 'I' || fgetc(fp) != 'F' || fgetc(fp) != 'F') {
		log_err("invalid file (invalid \"RIFF\" tag)");
		return -1;
	}
	remaining = fgetc(fp)+(fgetc(fp)<<8)+(fgetc(fp)<<16)+(fgetc(fp)<<24);
	if(filesize(file_name) < remaining+8) {
		log_err("invalid file (file shorter than described in the header)");
		return -1;
	}
	if(fgetc(fp) != 'W' || fgetc(fp) != 'A' || fgetc(fp) != 'V' || fgetc(fp) != 'E') {
		log_err("invalid file (invalid \"WAVE\" tag)");
		return -1;
	}
	remaining -= 4;

	while(remaining > 0) {
		block_label[0] = fgetc(fp);
		block_label[1] = fgetc(fp);
		block_label[2] = fgetc(fp);
		block_label[3] = fgetc(fp);
		block_remaining = fgetc(fp)+(fgetc(fp)<<8)+(fgetc(fp)<<16)+(fgetc(fp)<<24);
		if(remaining < block_remaining+8) {
			log_err("invalid file (block within the file longer than the described file size)");
			return -1;
		}
		remaining -= 8 + block_remaining + (block_remaining%2);
		switch(block_label[0]) {
			case 'f':
				if(block_label[1] != 'm' || block_label[2] != 't' || block_label[3] != ' ') {
					while(block_remaining > 0) {
						fgetc(fp);
						block_remaining--;
					}
					break; /* the whole case */
				}

				if(block_remaining != 16) {
					log_err("invalid format (format block size greater than expected for PCM)");
					return -1;
				}
				if(fgetc(fp) != 1 || fgetc(fp) != 0) {
					log_err("invalid format (format tag different from 0x0001 (PCM))");
					return -1;
				}

				available_channels = fgetc(fp) + (fgetc(fp)<<8);
				if(chosen_channel > available_channels) {
					log_err("invalid format (not enough channels available)");
					return -1;
				}

				out_waveform.samples_per_second = fgetc(fp)+(fgetc(fp)<<8)+(fgetc(fp)<<16)+(fgetc(fp)<<24);

				(void) (fgetc(fp)+(fgetc(fp)<<8)+(fgetc(fp)<<16)+(fgetc(fp)<<24));

				bytes_per_sample = (fgetc(fp)+(fgetc(fp)<<8))/available_channels;
				if (bytes_per_sample != 2) {
					log_err("invalid format (bits per sample not 16)");
					return -1;
				}

				(void) (fgetc(fp)+(fgetc(fp)<<8));

				found_format_block = 1;
			break;
			case 'd':
				if(block_label[1] != 'a' || block_label[2] != 't' || block_label[3] != 'a') {
					while(block_remaining > 0) {
						fgetc(fp);
						block_remaining--;
					}
					break; /* the whole case */
				}

				if(!found_format_block) {
					log_err("invalid file (no format block found before data block)");
					return -1;
				}

				out_waveform.data_length = block_remaining/(2*available_channels);
				out_waveform.amplitude_data = malloc(sizeof(int16_t)*wave.data_length);
				for(i = 0; i < wave.data_length; i++) {
					for(ch = 0; ch < available_channels; ch++) {
						if (ch == chosen_channel) {
							out_waveform.amplitude_data[i] = (fgetc(fp)+(fgetc(fp)<<8));
						} else {
							(void) (fgetc(fp)+(fgetc(fp)<<8));
						}
					}
				}
				if((block_remaining%2) == 1) (void) (fgetc(fp));
			break;


			default:
				while(block_remaining > 0) {
					fgetc(fp);
					block_remaining--;
				}
			break;
		}
	}
	
	return 0;
}


int write_int_wav_file(FILE* fp, waveform_t data) {
	if(fp == NULL) {
		log_err("File Pointer is a NULL pointer!");
		return -1;
	}
	if(data.amplitude_data == NULL) {
		log_err("Amplitude data is a NULL pointer!");
		return -1;
	}


	fputc('R', fp);
	fputc('I', fp);
	fputc('F', fp);
	fputc('F', fp);

	size_t writing_size = 36 + data.data_length*2;
	fputc((writing_size&0xFF), fp);
	fputc((writing_size&0xFF00)>>8, fp);
	fputc((writing_size&0xFF0000)>>16, fp);
	fputc((writing_size&0xFF000000)>>24, fp);

	fputc('W', fp);
	fputc('A', fp);
	fputc('V', fp);
	fputc('E', fp);

	fputc('f', fp);
	fputc('m', fp);
	fputc('t', fp);
	fputc(' ', fp);

	fputc(16, fp);
	fputc(0, fp);
	fputc(0, fp);
	fputc(0, fp);
	/* fmt */
	fputc(1, fp);
	fputc(0, fp);
	/* channels */
	fputc(1, fp);
	fputc(0, fp);

	fputc((data.samples_per_second&0xFF), fp);
	fputc((data.samples_per_second&0xFF00)>>8, fp);
	fputc((data.samples_per_second&0xFF0000)>>16, fp);
	fputc((data.samples_per_second&0xFF000000)>>24, fp);

	fputc(((data.samples_per_second*2)&0xFF), fp);
	fputc(((data.samples_per_second*2)&0xFF00)>>8, fp);
	fputc(((data.samples_per_second*2)&0xFF0000)>>16, fp);
	fputc(((data.samples_per_second*2)&0xFF000000)>>24, fp);

	fputc(2, fp);
	fputc(0, fp);

	fputc(16, fp);
	fputc(0, fp);

	fputc('d', fp);
	fputc('a', fp);
	fputc('t', fp);
	fputc('a', fp);

	fputc(((data.data_length*2)&0xFF), fp);
	fputc(((data.data_length*2)&0xFF00)>>8, fp);
	fputc(((data.data_length*2)&0xFF0000)>>16, fp);
	fputc(((data.data_length*2)&0xFF000000)>>24, fp);

	//printf("len %lli\n", data.data_length);

	for(int i = 0; i < data.data_length; i++) {
		fputc((data.amplitude_data[i]&0xFF), fp);
		fputc((data.amplitude_data[i]&0xFF00)>>8, fp);
	}
	
	return 0;
}


float duration(waveform_t form) {
    return (((float)form.data_length)/((float)form.samples_per_second));
}




int int_wave_to_floating(waveform_t in, floating_waveform_t* out) {
	if(out == NULL) {
		log_err("Floating Waveform outptr is a NULL pointer!");
		return -1;
	}
	
	out.sample_frequency_Hz = (float) in.samples_per_second;
	out.data_length = in.data_length;
	out.amplitude_data = malloc(out.data_length*sizeof(double));
	for(int i = 0; i < out.data_length; i++) {
		out.amplitude_data[i] = ((double) in.amplitude_data[i]) / (double) (1 << 15);
	}
	return 0;
}
int floating_wave_to_int(floating_waveform_t in, waveform_t* out) {
	if(out == NULL) {
		log_err("Waveform outptr is a NULL pointer!");
		return -1;
	}
	
	out.samples_per_second = (int) round(in.sample_frequency_Hz);
	out.data_length = in.data_length;
	out.amplitude_data = malloc(out.data_length*sizeof(int16_t));
	for(int i = 0; i < out.data_length; i++) {
		out.amplitude_data[i] = (int16_t) round(in.amplitude_data[i] * (1 << 15));
	}
	return 0;
}


static int checked_fread(void* ptr, size_t a, size_t b, FILE* fp) {
	size_t res = fread(ptr, a, b, fp);
	return (res != a * b);
}
static int checked_fwrite(void* ptr, size_t a, size_t b, FILE* fp) {
	size_t res = fwrite(ptr, a, b, fp);
	return (res != a * b);
}

int read_floating_wave(floating_waveform_t* wave, FILE* fp) {
	if(wave == NULL) {
		log_err("Floating Waveform is a NULL pointer!");
		return -1;
	}
	
	if(checked_fread(&wave[0].sample_frequency_Hz, sizeof(float), 1, fp)) {
		return -1;
	}
	if(checked_fread(&wave[0].data_length, sizeof(size_t), 1, fp)) {
		return -1;
	}
	
	wave[0].amplitude_data = malloc(wave[0].data_length*sizeof(double));
	if(wave[0].amplitude_data == NULL) return -1;
	
	if(checked_fread(wave[0].amplitude_data, sizeof(double), wave.data_length, fp)) {
		return -1;
	}
	return 0;
}

int write_floating_wave(floating_waveform_t wave, FILE* fp) {
	if(wave.amplitude_data == NULL) {
		log_err("Floating Waveform Amplitude is a NULL pointer!");
		return -1;
	}
	
	if(checked_fwrite(&wave.sample_frequency_Hz, sizeof(float), 1, fp)) {
		return -1;
	}
	if(checked_fwrite(&wave.data_length, sizeof(size_t), 1, fp)) {
		return -1;
	}
	if(checked_fwrite(wave.amplitude_data, sizeof(double), wave.data_length, fp)) {
		return -1;
	}
	return 0;
}



