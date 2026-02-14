#include "common.h"

typedef  float  point[2];
int main(int argc, char** argv) {
    simple_wav_t float_form = read_simple_wav(stdin);

	/* assume_halfed_input that's falling monotone, i.e. max ^= f/2 */
    assert(argc > 1);
    float max_hz = atof(argv[1]);
	float maxy = float_form.samples[0];
    
    int num_points = ((float)float_form.nr_sample_points)*max_hz / (float_form.frequency_in_hz/2);
	point *points = malloc(sizeof(point)*num_points);
	float avg_x = 0.0, avg_y = 0.0;
	for(int i = 1; i < num_points; i++) {
		points[i][0] = i / ((float)float_form.nr_sample_points);
		points[i][1] = float_form.samples[i] / maxy;
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
	
	printf("Up to index (%i/%i): Falloff is %f\n", num_points, float_form.nr_sample_points, num / denom);

    return 0;
}


