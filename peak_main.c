#include "common.h"

int main(int argc, char** argv) {
    simple_wav_t float_form = read_simple_wav(stdin);


    fprintf(stderr, "peaks: f = %f\n", float_form.frequency_in_hz);

    size_t len = float_form.nr_sample_points / 2;
    if(!is_power_of_2(len)) die("Data size collected not power of two!");
    float freq = float_form.frequency_in_hz;

    float* combined_frequencies = compute_complex_absolute_values(float_form.samples, len);
    float* normalized_frequencies = normalize_float_array(combined_frequencies, len);

    interval_t* intervals;
    size_t nr_intervals;
    get_sorted_iteratively_merged_interval_list_by_cutoff_step(normalized_frequencies, len/2, 0.01, &intervals, &nr_intervals);


    float* interval_formant_frequencies = calloc(nr_intervals, sizeof(float));
    freq = 44100.0f;
    for(int i = 0; i < nr_intervals; i++) {
        float upper_frequency_in_herz = freq * (((float)intervals[i].upper_index)/((float) len));
        float lower_frequency_in_herz = freq * (((float)intervals[i].lower_index)/((float) len));
        printf("F%i: %fHz-%fHz\n", i, lower_frequency_in_herz, upper_frequency_in_herz);
        interval_formant_frequencies[i] = (lower_frequency_in_herz + upper_frequency_in_herz) / 2;
    }
        //quick_sort_float(interval_formant_frequencies, nr_intervals);
    qsort(interval_formant_frequencies, nr_intervals, sizeof(float), float_cmp_qsort);

    float* formant_distances = calloc(nr_intervals-1, sizeof(float));
    float sum = 0.0f;
    for(int i = 1; i < nr_intervals; i++) {
        float distance = interval_formant_frequencies[i]-interval_formant_frequencies[i-1];
        printf("distance: %f\n", distance);
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



    free(interval_formant_frequencies);
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
