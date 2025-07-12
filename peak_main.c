#include "common.h"

int main(int argc, char** argv) {
    simple_wav_t float_form = read_simple_wav(stdin);


    //fprintf(stderr, "peaks: f = %f\n", float_form.frequency_in_hz);

    size_t len = float_form.nr_sample_points / 2;
    if(!is_power_of_2(len)) die("Data size collected not power of two!");
    float freq = float_form.frequency_in_hz;

    float* combined_frequencies = compute_complex_absolute_values(float_form.samples, len);
    float* normalized_frequencies = normalize_float_array(combined_frequencies, len);

    interval_t* intervals;
    size_t nr_intervals;
    get_sorted_iteratively_merged_interval_list_by_cutoff_step(normalized_frequencies, len/2, 0.01, &intervals, &nr_intervals);


    peak_t* peaks = calloc(nr_intervals, sizeof(peak_t));
    size_t nr_peaks = nr_intervals;

    /*
     *
int peak_by_freq_cmp_qsort(const void* pa, const void* pb);
int peak_by_height_cmp_qsort(const void* pa, const void* pb);

typedef struct peak_t {
    float freq;
    float height;
    int formant_nr;
    int merged_peaks;
} peak_t;
     */

    for(int i = 0; i < nr_intervals; i++) {
        float upper_frequency_in_herz = freq * (((float)intervals[i].upper_index)/((float) len));
        float lower_frequency_in_herz = freq * (((float)intervals[i].lower_index)/((float) len));
        peaks[i].freq = (lower_frequency_in_herz + upper_frequency_in_herz) / 2;
        peaks[i].height = 0.0f;
        for(int j = intervals[i].lower_index; j < intervals[i].upper_index; j++) {
            if(peaks[i].height < normalized_frequencies[j])
                peaks[i].height = normalized_frequencies[j];
        }
        peaks[i].merged_peaks = 0;
        //int oct, note, cents;
        //char* name = note_name(peaks[i].freq, &oct, &note, &cents);
        //if(name == NULL) continue;
        //printf("F%i: %s (%fHz-%fHz); y = %f\n", i, name, lower_frequency_in_herz, upper_frequency_in_herz, peaks[i].height);
        //free(name);
    }
        //quick_sort_float(interval_formant_frequencies, nr_intervals);
    qsort(peaks, nr_peaks, sizeof(peak_t), peak_by_freq_cmp_qsort);


#define MIN_CENTS_OF_DIFFERENCE 20

    for(int i = 0; i < nr_peaks; i++) {
        if(i != nr_peaks-1) {
            float curr_freq = peaks[i].freq;
            float next_freq = peaks[i+1].freq;
            float curr_height = peaks[i].height;
            float next_height = peaks[i+1].height;
            float distance = hz_to_octave(next_freq)-hz_to_octave(curr_freq);
            float cents = distance*12*100;
            bool should_be_merged = (cents < MIN_CENTS_OF_DIFFERENCE);

            //printf("curf=%f,nxf=%f,curh=%f,nxh=%f,dis=%f,cents=%f,sbm=%i\n", curr_freq, next_freq, curr_height, next_height, distance, cents, should_be_merged);

            if(should_be_merged) {
                if(curr_height > next_height) {
                    peaks[i+1] = peaks[i];
                }
                peaks[i+1].merged_peaks++;
                peaks[i].freq = -1.0f;
            }
        }
    }


    qsort(peaks, nr_peaks, sizeof(peak_t), peak_by_height_cmp_qsort);
    int nr = 0;
    for(int i = nr_peaks-1; i >= 0; i--) {
        if(peaks[i].freq == -1.0f) continue;
        peaks[i].formant_nr = nr;
        nr++;
    }

    qsort(peaks, nr_peaks, sizeof(peak_t), peak_by_freq_cmp_qsort);

    for(int i = 0; i < nr_peaks; i++) {

        int oct, note, cents;
        if(peaks[i].freq == -1.0f) continue;
        char* name = note_name(hz_to_octave(peaks[i].freq), &oct, &note, &cents);
        if(name == NULL) continue;
        printf("F%3i: %s; y = %7f (mp:%i)\n", peaks[i].formant_nr, name, peaks[i].height, peaks[i].merged_peaks);
        free(name);

    }


    float* formant_distances = calloc(nr_intervals-1, sizeof(float));
    float sum = 0.0f;
    for(int i = 1; i < nr_intervals; i++) {
        if(peaks[i].freq == -1.0f) continue;
        float distance = peaks[i].freq-peaks[i-1].freq;
        //printf("distance: %f\n", distance);
        sum += distance;
        formant_distances[i] = distance;
    }
    printf("\nAverage distance: %f\n", sum/nr_intervals);
    printf("VTL in m = %f\n", 343.0/(2*(sum/nr_intervals)));


    //quick_sort_float(formant_distances, nr_intervals-1);
    qsort(formant_distances, nr_intervals-1, sizeof(float), float_cmp_qsort);

    printf("\nMean distance: %f\n", formant_distances[(nr_intervals-1)/2]);
    printf("Mean VTL in m = %f\n", 343.0/(2*(formant_distances[(nr_intervals-1)/2])));

    float min_sensible_distance = 100.0f; // in Hz
    int min_idx = -1;
    for(int i = 0; i < nr_intervals-1; i++) {
        if(peaks[i].freq == -1.0f) continue;
        if(formant_distances[i] > min_sensible_distance) {
            min_idx = i;
            break;
        }
    }
    if(min_idx >= 0) {
        int mean_sensible_index = ((nr_intervals-1) + min_idx)/2;

        printf("\nMean sensible distance: %f\n", formant_distances[mean_sensible_index]);
        printf("Mean sensible VTL in m = %f\n", 343.0/(2*(formant_distances[mean_sensible_index])));
        printf("Percentage of sensible distances: %f\n", (1 - ((float)mean_sensible_index/((float) nr_intervals-1))));
    }


    free(peaks);
    free(formant_distances);

    return 0;
}

/*

        for(int i = 0; i < state.nr_intervals && i < 10; i++) {
            float frequency_in_herz = state.freq * (((float)state.intervals[i].upper_index)/((float) state.len));
            float octave = log2(frequency_in_herz/27.5);
            float x_value = octave/10;
            float xval = dia.base_x + (dia.max_x)*x_value;
            dash[0].x = xval;
            dash[0].y = dia.base_y - dia.max_y - 50.0f;
            dash[1].x = xval;
            dash[1].y = dia.base_y + 50.0f;
            DrawSplineLinear(dash, 2, 1.0f, ORANGE);

            frequency_in_herz = state.freq * (((float)state.intervals[i].lower_index)/((float) state.len));
            octave = log2(frequency_in_herz/27.5);
            x_value = octave/10;
            xval = dia.base_x + (dia.max_x)*x_value;
            dash[0].x = xval;
            dash[0].y = dia.base_y - dia.max_y - 50.0f;
            dash[1].x = xval;
            dash[1].y = dia.base_y + 50.0f;
            DrawSplineLinear(dash, 2, 1.0f, PURPLE);
        }



*/
