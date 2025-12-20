
#ifndef COMMON_H
#define COMMON_H

#ifdef HAS_RAYLIB
#include "raylib.h"
#endif

#include "alad.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <limits.h>
#include <time.h>
#ifndef M_PI
#    define M_PI 3.14159265358979323846
#endif




/* gauss.c */


typedef struct matrix_t {
    int width, height;
    float* data;
} matrix_t;

matrix_t create_matrix(int width, int height);
void destroy_matrix(matrix_t mat);
matrix_t copy_matrix(matrix_t mat);
matrix_t gaussian(matrix_t mat_input, matrix_t v);

void print_matrix(matrix_t mat);

float get(matrix_t mat, int col, int row);
void set(matrix_t mat, int col, int row, float val);
matrix_t invert_matrix(matrix_t mat_input);
matrix_t matrix_multiply(matrix_t a, matrix_t b);


/* bmp.c */
void write_bitmap_data(char* file_name, uint8_t* red_data, uint8_t* green_data, uint8_t* blue_data, uint8_t* alpha_data, size_t len, int width, int height, float DPI);


/* wav.c */

typedef struct {
	int samples_per_second;
	int16_t* amplitude_data;
	size_t data_length;
} waveform_t;
/*
	Reads a 16 bit PCM Wave file bytewise; kills the program if it doesn't work (gory!)
	For documentation see the specs and overview at
	https://www.mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/WAVE.html
*/
waveform_t read_amplitude_data(char* file_name, int chosen_channel);
void destroy_waveform(waveform_t* wave);

void write_amplitude_data(char* file_name, waveform_t data);
float duration(waveform_t form);


/* lpc.c */

float lpc_pole_function(matrix_t lpc_vector, float x);
matrix_t lpc_float(double* data, size_t len, int nr_formants);

/* formant.c */

typedef struct formant_params_t {
    float filter_strength;
    float window_param1;
    float window_param2;
    float min_freq;
    float max_bw;
    float min_imag;
} formant_params_t;
extern formant_params_t g_params;
void init_formant_params(void);
int find_formants(waveform_t form, int nr_formants, matrix_t* formant_values, matrix_t* formant_bandwidths, bool do_extra_division);


/* root.c */
void poly_complex_solve(const double *polynomial_coefficients, size_t nr_of_terms, double* polynomial_roots, double* working_matrix);

/* fft.c */
/* returns a pointer to be freed with free() (i.e. gives ownership)
   the array is made up of complex numbers as in pairs, with real first and then imaginary
   this means that the full frequency part is the sum of squares of both */
float* fft_power_of_two(float* data, size_t len);
float* ifft_power_of_two(float* data, size_t len);




/* cutoff_intervals.c */

typedef struct interval_t {
    size_t lower_index;
    size_t upper_index;
} interval_t;


void find_intervals_above_cutoff(float* data, size_t len, float cutoff, interval_t** intervals_out, size_t* nr_intervals_out);

void print_intervals(interval_t* intervals, size_t nr_intervals);

void merge_interval_lists(interval_t* old_intervals, size_t nr_old_intervals, interval_t* new_intervals, size_t nr_new_intervals, interval_t** intervals_out, size_t* nr_intervals_out);

void sort_interval_lists(interval_t* intervals, size_t nr_intervals);


void get_sorted_iteratively_merged_interval_list_by_cutoff_step(float* data, size_t len, float cutoff_step, interval_t** intervals_out, size_t* nr_intervals_out);



/* util.c */
void write_dbl_array(double* data, size_t len, const char* filename);
void write_float_array(float* data, size_t len, const char* filename);
void quick_sort_float(float* array, size_t len);
size_t filesize(const char* path);
void die(const char* str);
bool is_power_of_2(uint32_t x);
uint32_t truncate_power_of_2(uint32_t x);
float clamp(float val, float min, float max);
float *transform_to_complex_array(const int16_t* old_array, size_t length);
float* compute_complex_absolute_values(const float* old_array, size_t length);
void maximum_abs_value_and_position_int_array(const int16_t* array, size_t length, int* max_array_index, int16_t* max_array_value);
void maximum_abs_value_and_position_float_array(const float* array, size_t length, int* max_array_index, float* max_array_value);
float *normalize_int_array(int16_t* old_array, size_t length);
float *normalize_float_array(float* old_array, size_t length);
int16_t *transform_complex_to_int_array(const float* old_array, size_t length);
#ifdef HAS_RAYLIB
Vector2* create_graph_from_float_array (float* array, size_t length, float base_x, float base_y, float max_x, float max_y);
#endif
int float_cmp_qsort(const void* pa, const void* pb);
int formant_by_freq_cmp_qsort(const void* pa, const void* pb);

double hz_to_octave(double freq_in_hz);
char* note_name(double freq_in_hz, int* out_oct, int* out_note, int* out_cents);

int peak_by_freq_cmp_qsort(const void* pa, const void* pb);
int peak_by_height_cmp_qsort(const void* pa, const void* pb);

typedef struct peak_t {
    interval_t underlying_interval;
    float freq;
    float height;
    int formant_nr;
    int merged_peaks;
    float rolloff_v;
    int min_index;
} peak_t;
void debug_peaks(peak_t* peaks, size_t nr_peaks);

char** split(const char* str, size_t len, char sep, int* out_num_strings);
float *transform_float_to_complex_array(const float* old_array, size_t length);


typedef struct formant_t {
    double freq, bw;
    bool stable_and_keep;
} formant_t;


/* direct_peak.c */
int* calculate_peaks(float* data, size_t len, const char* filename);
int* calculate_peaks_dbl(double* data, size_t len, const char* filename);



void sleep_us(uint32_t us);















/* simple_wav.c */

typedef struct simple_wav_t {
    float frequency_in_hz;
    size_t nr_sample_points;
    float* samples;
    size_t nr_peaks;
    peak_t* peaks;
} simple_wav_t;
simple_wav_t read_simple_wav(FILE* fp);
void write_simple_wav(FILE* fp, simple_wav_t data);





/* f80.c */

void convert_to_extended_float_be(double val, char* outptr);
double convert_from_extended_float_be(char* inptr);


/* lpc.c */
float* lpc_coefficients_rosa(float* data, size_t len, int order);
double* lpc_coefficients_rosa_double(double* data, size_t len, int order);

/* marple_alg_2.c */
int ar_params (double *x_in /* data */,
     int n /* num_values */,
     int mmax /* nr_formants */,
     float tol1, float tol2 /* tolerances */,
     int* out_m /* out_calculated */,
     double *a_in /* out_ar_params, has to have nr_formants space */,
     float *out_e /* out_pred_error_energy_order_m */,
     float *out_e0 /* out_twice_total_energy */
     );
/* informant_algs.c */

double* autocorr_solve(const double *data, int length, int lpcOrder, double *pGain, size_t* out_nr_formants);
double* Covar_solve(const double *data, int length, int lpcOrder, double *pGain, size_t* out_nr_formants);
double* Burg_solve(const double *x, int length, int lpcOrder, double *pGain, size_t* out_nr_formants);




/* r_formant_code */
void r_find_formants(double* sound, size_t len, double frequency, int order, int maxbw, int minformant, double* formants, double* bws, bool* is_selected);

#endif
