#include "common.h"



simple_wav_t read_simple_wav(FILE* fp) {
    simple_wav_t ret;
    fread(&ret.frequency_in_hz, sizeof(float), 1, fp);
    fread(&ret.nr_sample_points, sizeof(size_t), 1, fp);
    ret.samples = calloc(ret.nr_sample_points, sizeof(float));
    fread(ret.samples, sizeof(float), ret.nr_sample_points, fp);
    return ret;
}

void write_simple_wav(FILE* fp, simple_wav_t data) {
    fwrite(&data.frequency_in_hz, sizeof(float), 1, fp);
    fwrite(&data.nr_sample_points, sizeof(size_t), 1, fp);
    fwrite(data.samples, sizeof(float), data.nr_sample_points, fp);
}

