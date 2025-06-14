

we need:

state.freq

    size_t len = truncate_power_of_2(snippet.data_length) >> chosen_reduction_power;
    float freq = ((float)snippet.samples_per_second);

    if(!is_power_of_2(len)) die("Data size collected not power of two!");

    float* amplitudes = transform_to_complex_array(snippet.amplitude_data, len);
    float* frequencies = fft_power_of_two(amplitudes, len*2);
    float* combined_frequencies = compute_complex_absolute_values(frequencies, len);
    float* normalized_frequencies = normalize_float_array(combined_frequencies, len);

    interval_t* intervals;
    size_t nr_intervals;
    get_sorted_iteratively_merged_interval_list_by_cutoff_step(normalized_frequencies, len/2, 0.01, &intervals, &nr_intervals);

    for(int i = 0; i < nr_intervals; i++) {
        float upper_frequency_in_herz = freq * (((float)intervals[i].upper_index)/((float) len));
        float lower_frequency_in_herz = freq * (((float)intervals[i].lower_index)/((float) len));
        printf("F%i: %fHz-%fHz\n", i, lower_frequency_in_herz, upper_frequency_in_herz);
        interval_formant_frequencies[i] = (lower_frequency_in_herz + upper_frequency_in_herz) / 2;
    }

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




