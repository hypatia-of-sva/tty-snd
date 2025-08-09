#include "common.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

void write_dbl_array(double* data, size_t len, const char* filename) {
    FILE* fd = fopen(filename, "wb");
    for(int i = 0; i < len; i++) {
        fprintf(fd, "[%i] = %f,\n", i, data[i]);
    }
    fclose(fd);
}

void write_float_array(float* data, size_t len, const char* filename) {
    FILE* fd = fopen(filename, "wb");
    for(int i = 0; i < len; i++) {
        fprintf(fd, "[%i] = %f,\n", i, data[i]);
    }
    fclose(fd);
}

float median3(float a, float b, float c) {
    if(a >= b && b >= c) return b;
    if(a >= c && c >= b) return c;
    if(b >= a && a >= c) return a;
    if(b >= c && c >= a) return c;
    if(c >= b && b >= a) return b;
    if(c >= a && a >= b) return a;
    die("how?");
    return b;
}


int float_cmp_qsort(const void* pa, const void* pb) {
    float a = ((float*)pa)[0];
    float b = ((float*)pb)[0];
    if(a == b) return 0;
    if(a > b) return 1;
    else return -1;
}


int peak_by_freq_cmp_qsort(const void* pa, const void* pb) {
    peak_t a = ((peak_t*)pa)[0];
    peak_t b = ((peak_t*)pb)[0];
    if(a.freq == b.freq) return 0;
    if(a.freq > b.freq) return 1;
    else return -1;
}
int peak_by_height_cmp_qsort(const void* pa, const void* pb) {
    peak_t a = ((peak_t*)pa)[0];
    peak_t b = ((peak_t*)pb)[0];
    if(a.height == b.height) return 0;
    if(a.height > b.height) return 1;
    else return -1;
}


void quick_sort_float(float* array, size_t len) {
    /* iterative quicksort, adapted from https://www.geeksforgeeks.org/iterative-quick-sort/ */
    int* stack = calloc(len,sizeof(int));
    stack[0] = 0;
    stack[1] = len-1;
    int top = 1;
    while (top >= 0) {
        int h = stack[top--];
        int l = stack[top--];

        /* partition: */
        float x = array[h]; /* pivot */
        int i = l-1;
        for(int j = l; j < h; j++) {
            if(array[j] <= x) {
                i++;
                float tmp = array[i];
                array[i] = array[j];
                array[j] = tmp;
            }
        }
        float tmp = array[i+1];
        array[i+1] = array[h];
        array[h] = tmp;

        if (i > l) {
            stack[++top] = l;
            stack[++top] = i;
        }
        if (i+2 < h) {
            stack[++top] = i+2;
            stack[++top] = h;
        }
    }
    free(stack);
}
Vector2* create_graph_from_float_array (float* array, size_t length, float base_x, float base_y, float max_x, float max_y) {
    Vector2* graph = malloc(length*sizeof(Vector2));
    for(int i = 0; i < length; i++) {
        graph[i].x = base_x + (max_x/length)*i;
        graph[i].y = base_y - max_y*array[i];
    }
    return graph;
}
float *normalize_int_array(int16_t* old_array, size_t length) {
    int max_pos; int16_t max_to_normalize_by = 0;
    maximum_abs_value_and_position_int_array(old_array, length, &max_pos, &max_to_normalize_by);
    float* new_array = malloc(length * sizeof(float));
    for(int i = 0; i < length; i++) {
        new_array[i] = ((float)old_array[i])/((float)max_to_normalize_by);
    }
    return new_array;
}
float *normalize_float_array(float* old_array, size_t length) {
    int max_pos; float max_to_normalize_by = 0;
    maximum_abs_value_and_position_float_array(old_array, length, &max_pos, &max_to_normalize_by);
    float* new_array = malloc(length * sizeof(float));
    for(int i = 0; i < length; i++) {
        new_array[i] = (old_array[i])/(max_to_normalize_by);
    }
    return new_array;
}
void maximum_abs_value_and_position_int_array(const int16_t* array, size_t length, int* max_array_index, int16_t* max_array_value) {
    max_array_index[0] = 0;
    max_array_value[0] = 0;
    for(int i = 0; i < length; i++) {
        if(abs(array[i]) > abs(max_array_value[0])) {
            max_array_value[0] = abs(array[i]);
            max_array_index[0] = i;
        }
    }
}
void maximum_abs_value_and_position_float_array(const float* array, size_t length, int* max_array_index, float* max_array_value) {
    max_array_index[0] = 0;
    max_array_value[0] = 0.0f;
    for(int i = 0; i < length; i++) {
        if(fabsf(array[i]) > fabsf(max_array_value[0])) {
            max_array_value[0] = fabsf(array[i]);
            max_array_index[0] = i;
        }
    }
}
int16_t *transform_complex_to_int_array(const float* old_array, size_t length) {
    int16_t* new_array = malloc(length * sizeof(int16_t));
    for(int i = 0; i < length; i++) {
        new_array[i] = ceil(clamp(old_array[2*i], -32768.0f, 32768.0f));
    }
    return new_array;
}
float *transform_to_complex_array(const int16_t* old_array, size_t length) {
    float* new_array = malloc(length * 2 * sizeof(float));
    for(int i = 0; i < length; i++) {
        new_array[2*i] = (float) old_array[i];
        new_array[2*i+1] = 0.0f;
    }
    return new_array;
}

float *transform_float_to_complex_array(const float* old_array, size_t length) {
    float* new_array = malloc(length * 2 * sizeof(float));
    for(int i = 0; i < length; i++) {
        new_array[2*i] = old_array[i];
        new_array[2*i+1] = 0.0f;
    }
    return new_array;
}


float* compute_complex_absolute_values(const float* old_array, size_t length) {
    float* new_array = malloc(length * sizeof(float));
    for(int i = 0; i < length; i++) {
        new_array[i] = sqrtf(old_array[2*i]*old_array[2*i] + old_array[2*i+1]*old_array[2*i+1]);
    }
    return new_array;
}
float clamp(float val, float min, float max) {
    if(val < min) return min;
    if(val > max) return max;
    return val;
}

size_t filesize(const char* path) {
    struct stat buf;
    stat(path, &buf);
    off_t size = buf.st_size;
    return size;
}
void die(const char* str) {
	fprintf(stderr, str);
	exit(EXIT_FAILURE);
}
bool is_power_of_2(uint32_t x) {
    return x > 0 && !(x & (x-1));
}
uint32_t truncate_power_of_2(uint32_t x) {
    for(int bit = 31; bit > 0; bit--) {
        uint32_t pow = (1U << bit);
        if(x>=pow) return pow;
    }
    return 0;
}

#define C0_Hz 16.351597831287414

double hz_to_octave(double freq_in_hz) {
    return log2(freq_in_hz/C0_Hz);
}

char* note_name(double octave, int* out_oct, int* out_note, int* out_cents) {
    if(octave < 0 || octave > 10) return NULL;
    int octave_nr = floor(octave);
    int closest_note = lround((octave - octave_nr)*12);
    if(closest_note == 12) {
        closest_note = 0;
        octave_nr++;
    }
    int cents = lround((((octave - octave_nr)*12)-closest_note)*100);
    //printf("O\t\t%f, %f, %i, %i, %i\n", freq_in_hz, octave, octave_nr, closest_note, cents);
    char* note_names[12] = {
        "C ", "C#", "D ", "D#", "E ", "F ", "F#", "G ", "G#", "A ", "A#", "B "
    };

    //printf("\t\t%f, %f, %i, %i, %i\n", freq_in_hz, octave, octave_nr, closest_note, cents);

    if(out_oct != NULL)
        out_oct[0] = octave_nr;
    if(out_note != NULL)
        out_note[0] = closest_note;
    if(out_cents != NULL)
        out_cents[0] = cents;


    assert(octave_nr >= 0 && octave_nr <= 10);
    assert(closest_note >= 0 && closest_note <= 11);
    assert(cents >= -50 && octave_nr <= 50);


    char* buf = calloc(9,1);
    buf[0] = note_names[closest_note][0];
    buf[1] = note_names[closest_note][1];
    if(octave_nr == 10) {
        buf[2] = '1';
        buf[3] = '0';
    } else {
        buf[2] = '0'+octave_nr;
        buf[3] = ' ';
    }
    if(cents == 0) {
        buf[4] = '+';
        buf[5] = '-';
        buf[6] = '0';
        buf[7] = 'c';
    } else if(cents > 0) {
        buf[4] = '+';
        buf[5] = '0'+(abs(cents)/10);
        buf[6] = '0'+(abs(cents)%10);
        buf[7] = 'c';
    } else {
        buf[4] = '-';
        buf[5] = '0'+(abs(cents)/10);
        buf[6] = '0'+(abs(cents)%10);
        buf[7] = 'c';
    }
    buf[8] = '\0';
    return buf;
}












char** split(const char* str, size_t len, char sep, int* out_num_strings) {
  assert(out_num_strings != NULL);
  out_num_strings[0] = 1;
  bool in_sep = (str[0] == sep);
  for(int i = 0; i < len; i++) {
    if(str[i] == sep && !in_sep && i != len-1) {
      out_num_strings[0]++;
      in_sep = true;
    } else {
      in_sep = false;
    }
  }

  char** split_strings = calloc(out_num_strings[0], sizeof(char*));
  int str_idx = 0;
  in_sep = (str[0] == sep);
  int first_char = 0;
  for(int i = 0; i < len; i++) {
    if(str[i] == sep && !in_sep) {
      size_t new_len = i - first_char + 1;
      split_strings[str_idx] = malloc(new_len);
      memmove(split_strings[str_idx], &str[first_char], new_len-1);
      split_strings[str_idx][new_len-1] = '\0';
      str_idx++;
      in_sep = true;
    } else if (str[i] != sep) {
      if(in_sep) first_char = i;
      in_sep = false;
    }
  }
  if(!in_sep) {
    size_t new_len = (len) - first_char+1;
    split_strings[str_idx] = malloc(new_len);
    memmove(split_strings[str_idx], &str[first_char], new_len-1);
    split_strings[str_idx][new_len-1] = '\0';
  }
  assert(str_idx+1 == out_num_strings[0]);

  return split_strings;
}
