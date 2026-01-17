
#include "common.h"

typedef  float  point[2];

int main(int argc, char** argv) {
    simple_wav_t float_form = read_simple_wav(stdin);

	int maxy = 0;
	for(int i = 0; i < float_form.nr_sample_points / 2; i++) {
		if (float_form.samples[i] > float_form.samples[maxy])
			maxy = i;
	}
	float maxy_freq = float_form.frequency_in_hz * ((float)maxy / (float)float_form.nr_sample_points );
	printf("max y: %i, freq: %f\n", maxy, maxy_freq);

	int num_points = floor(((float)float_form.nr_sample_points) / (2.0*(float)maxy));
	point *points = malloc(sizeof(point)*num_points);
	float avg_x = 0.0, avg_y = 0.0;
	for(int i = 1; i < num_points; i++) {
		points[i][0] = i*maxy_freq;
		points[i][1] = float_form.samples[i]*maxy;
		avg_x += points[i][0];
		avg_y += points[i][1];
	}
	avg_x /= num_points;
	avg_y /= num_points;
	
	
	float num = 0.0, denom = 0.0;
	for(int i = 1; i < num_points; i++) {
		num += (points[i][1] - avg_y) * points[i][0];
		denom += (points[i][0] - avg_x) * points[i][0];
	}
	
	printf("Slope fall rate: %f\n", -num/denom);

    return 0;
}
