#include "common.h"


/* asserted fgetc */
uint8_t afgetc(FILE* fd) {
    int c = fgetc(fd);
    assert(c != EOF);
    return (uint8_t)c;
}
/* asserted fputc */
void afputc(uint8_t c, FILE* fd) {
    assert(fputc(c, fd) != EOF);
}
uint16_t read_u16be(FILE* fd)  {
    return ((uint16_t)afgetc(fd)<<8) + ((uint16_t)afgetc(fd));
}
uint32_t read_u32be(FILE* fd)  {
    uint32_t c1 = ((uint32_t)afgetc(fd))<<24;
    uint32_t c2 = ((uint32_t)afgetc(fd))<<16;
    uint32_t c3 = ((uint32_t)afgetc(fd))<<8;
    uint32_t c4 = ((uint32_t)afgetc(fd));
    return (c1+c2+c3+c4);
}
void write_u16be(FILE* fd, uint16_t val) {
    afputc((val&0xFF00)>>8, fd);
    afputc((val&0xFF), fd);
}
void write_u32be(FILE* fd, uint32_t val) {
    afputc((val&0xFF000000)>>24, fd);
    afputc((val&0xFF0000)>>16, fd);
    afputc((val&0xFF00)>>8, fd);
    afputc((val&0xFF), fd);
}
int16_t read_i16be(FILE* fd)  {
    return (int16_t)read_u16be(fd);;
}
int32_t read_i32be(FILE* fd)  {
    return (int32_t)read_u32be(fd);
}
void write_i16be(FILE* fd, int16_t val) {
    write_u16be(fd, (uint16_t)val);
}
void write_i32be(FILE* fd, int32_t val) {
    write_u32be(fd, (uint32_t)val);
}


float read_f32be(FILE* fd)  {
    uint32_t dataval = ((uint32_t)fgetc(fd)<<24) + ((uint32_t)fgetc(fd)<<16) + ((uint32_t)fgetc(fd)<<8) + ((uint32_t)fgetc(fd));
    return ((float*)&dataval)[0];
}

void write_f32be(FILE* fd, float val) {
    uint32_t dataval = ((uint32_t*)&val)[0];
    afputc((dataval&0xFF000000)>>24, fd);
    afputc((dataval&0xFF0000)>>16, fd);
    afputc((dataval&0xFF00)>>8, fd);
    afputc((dataval&0xFF), fd);
}

#define check_input(fd, text) \
    for(int i = 0; i < strlen(text); i++) { \
        assert(fgetc(fd) == text[i]);       \
    }

/*
void check_input(FILE* fd, const char* text) {
    for(int i = 0; i < strlen(text); i++) {
        assert(fgetc(fd) == text[i]);
    }
}

*/






/*
 * AIFF header:
 *
 * "FORM"
 * i32be = len of the rest; i.e. filesize-8byte = 84 + sizeof appdata + sizeof data
 * "AIFC"
 * "FVER\0\0\0\4\xA2\x80\x51\x40"
 * "COMM"
 * "\0\0\0\24"
 * i16be = numChannels = 1
 * u32be = numSampleFrames = samples/channel
 * i16be = sampleSize = bits/sample = 32
 *              (i.e. full size is sampleSize * numSampleFrames * numChannels)
 * "extended" sampleRate = sample_frames/sec; stored as the 10 byte / 80 bit format
 * "fl32\0\0"
 * "APPL"
 * i32be = sizeof appdata + 12
 * "stoc"
 * "\7tty-snd"
 * i8 appdata[]
 * "SSND"
 * i32be = sizeof data + 8
 * "\0\0\0\0\0\0\0\0"
 * i8 data[] which will include floats
 */






char* appdata_basic = "VER: 0";
char* appdata_peaks_intro = "VER: 1 peaks=";






char* format_peak_structs(peak_t* peaks, size_t nr_peaks, int* bufsize_out) {
    int bufsize = 0;
    for(int i = 0; i < nr_peaks; i++) {
        bufsize += snprintf(NULL, 0, "%f:%f:%i:%i:%f:%i:%zu:%zu|",
            peaks[i].freq, peaks[i].height, peaks[i].formant_nr, peaks[i].merged_peaks, peaks[i].rolloff_v, peaks[i].min_index, peaks[i].underlying_interval.lower_index, peaks[i].underlying_interval.upper_index);
    }
    char* buf = calloc(bufsize+1,1);
    int cur_index = 0;
    for(int i = 0; i < nr_peaks; i++) {
        cur_index += snprintf(&(buf[cur_index]), bufsize, "%f:%f:%i:%i:%f:%i:%zu:%zu|",
            peaks[i].freq, peaks[i].height, peaks[i].formant_nr, peaks[i].merged_peaks, peaks[i].rolloff_v, peaks[i].min_index, peaks[i].underlying_interval.lower_index, peaks[i].underlying_interval.upper_index);
    }
    bufsize_out[0] = bufsize+1;
    return buf;
}
void parse_peak_structs(char* data, size_t len, peak_t** peaks_out, size_t* nr_peaks_out) {
    int nr_peaks;
    char** peak_strs = split(data, len, '|', &nr_peaks);
    peak_t* peaks = calloc(nr_peaks, sizeof(peak_t));
    for(int i = 0; i < nr_peaks; i++) {
        sscanf(peak_strs[i], "%f:%f:%i:%i:%f:%i:%zu:%zu",
            &peaks[i].freq, &peaks[i].height, &peaks[i].formant_nr, &peaks[i].merged_peaks, &peaks[i].rolloff_v, &peaks[i].min_index, &peaks[i].underlying_interval.lower_index, &peaks[i].underlying_interval.upper_index);
        free(peak_strs[i]);
    }
    free(peak_strs);
    peaks_out[0] = peaks;
    nr_peaks_out[0] = nr_peaks;

/*

    int nr_peaks;
    for(int i = 0; i < len; i++) {
        if(data[i] == '|') nr_peaks++;
    }
    peak_t* peaks = calloc(nr_peaks, sizeof(peak_t));

    int cur_index = 0;
    for(int i = 0; i < nr_peaks; i++) {
        int this_time_read = 0;
        sscanf(&(data[cur_index]), "%f:%f:%i:%i:%f:%i|%n",
            &peaks[i].freq, &peaks[i].height, &peaks[i].formant_nr, &peaks[i].merged_peaks, &peaks[i].rolloff_v, &peaks[i].min_index, &this_time_read);
        fprintf(stderr, "read: %i of %zu (round %i of %i)\n", cur_index, len, i, nr_peaks);
        cur_index += this_time_read;
    }

    peaks_out[0] = peaks;
    nr_peaks_out[0] = nr_peaks;
*/
}





void write_simple_wav(FILE* fp, simple_wav_t data) {
    if (fp == stdout) SET_BINARY_MODE(stdout);
    char* appdata = NULL;
    if(data.nr_peaks == 0 || data.peaks == NULL) {
        appdata = appdata_basic;
    } else {
        int struct_bufsize = 0;
        char* struct_buf = format_peak_structs(data.peaks, data.nr_peaks, &struct_bufsize);
        size_t allocsize = strlen(appdata_peaks_intro) + struct_bufsize;
        appdata = calloc(allocsize, 1); /* struct_bufsize already includes NUL */
        memmove(appdata, appdata_peaks_intro, strlen(appdata_peaks_intro));
        memmove(&appdata[strlen(appdata_peaks_intro)], struct_buf, struct_bufsize);
        free(struct_buf);
    }
    /*fprintf(stderr, "out: appdata=|%s|\n", appdata);
    fprintf(stderr, "out: peaknr = %zu, ptr = %p\n", data.nr_peaks, data.peaks); */


    /* basic AIFF header */
    fprintf(fp, "FORM");
    write_i32be(fp, 84 + strlen(appdata) + sizeof(float)*data.nr_sample_points);
    fprintf(fp, "AIFC");
    fprintf(fp, "FVER");
    write_i32be(fp, 4);
    afputc(0xA2, fp);
    afputc(0x80, fp);
    afputc(0x51, fp);
    afputc(0x40, fp);
    fprintf(fp, "COMM");
    write_i32be(fp, 24);
    write_i16be(fp, 1); /* 1 channel */
    write_i32be(fp, data.nr_sample_points);
    write_i16be(fp, 32);
    char extended[10] = {0};
    convert_to_extended_float_be((double) data.frequency_in_hz, &extended[0]);
    fwrite(&extended[0], 10, 1, fp);
    fprintf(fp, "fl32");
    afputc(0, fp);
    afputc(0, fp);
    fprintf(fp, "APPL");

    write_i32be(fp, strlen(appdata)+12);
    fprintf(fp, "stoc");
    fprintf(fp, "\7tty-snd"); /* application id */
    fwrite(appdata, 1, strlen(appdata), fp);


    fprintf(fp, "SSND");
    size_t ssnd_block_size = sizeof(float)*data.nr_sample_points+8;
    write_i32be(fp, ssnd_block_size);
    write_i32be(fp, 0);
    write_i32be(fp, 0);
    /*fwrite(data.samples, sizeof(float), data.nr_sample_points, fp); */
    for(int i = 0; i < data.nr_sample_points; i++) {
        write_f32be(fp, data.samples[i]);
    }
}

simple_wav_t read_simple_wav(FILE* fp) {
    if (fp == stdin) SET_BINARY_MODE(stdin);
    simple_wav_t ret = {0};
    int32_t size_to_read;

    check_input(fp, "FORM");
    size_to_read = read_i32be(fp);
    assert(size_to_read > 0);
    check_input(fp, "AIFC");                        size_to_read -= 4;
    check_input(fp, "FVER");                        size_to_read -= 4;
    assert(read_i32be(fp) == 4);                    size_to_read -= 4;
    assert(afgetc(fp) == (unsigned char) 0xA2);     size_to_read -= 1;
    assert(afgetc(fp) == (unsigned char) 0x80);     size_to_read -= 1;
    assert(afgetc(fp) == (unsigned char) 0x51);     size_to_read -= 1;
    assert(afgetc(fp) == (unsigned char) 0x40);     size_to_read -= 1;
    check_input(fp, "COMM");                        size_to_read -= 4;
    assert(read_i32be(fp) == 24);                   size_to_read -= 4;
    assert(read_i16be(fp) == 1);                    size_to_read -= 2;
    ret.nr_sample_points = read_i32be(fp);          size_to_read -= 4;

    size_t expected_size = sizeof(float)*ret.nr_sample_points+8;

    assert(read_i16be(fp) == 32);                   size_to_read -= 2;
    char extended[10];
    fread(&extended[0], 10, 1, fp);                 size_to_read -= 10;
    ret.frequency_in_hz = (float) convert_from_extended_float_be(&extended[0]);
    check_input(fp, "fl32");                        size_to_read -= 4;
    assert(afgetc(fp) == (char) 0);                 size_to_read -= 1;
    assert(afgetc(fp) == (char) 0);                 size_to_read -= 1;
    check_input(fp, "APPL");                        size_to_read -= 4;

    int32_t appdata_size;
    appdata_size = read_i32be(fp)-12;               size_to_read -= 4;

    check_input(fp, "stoc");                        size_to_read -= 4;
    check_input(fp, "\7tty-snd");                   size_to_read -= 8;
    char* appdata_buf = malloc(appdata_size);
    fread(appdata_buf, 1, appdata_size, fp);         size_to_read -= appdata_size;

    if(appdata_size == strlen(appdata_basic)) {
        assert(strncmp(appdata_basic, appdata_buf, appdata_size) == 0);
    } else {
        assert(appdata_size > strlen(appdata_peaks_intro));
        assert(strncmp(appdata_buf, appdata_peaks_intro, strlen(appdata_peaks_intro)) == 0);
        parse_peak_structs(&appdata_buf[strlen(appdata_peaks_intro)], appdata_size-strlen(appdata_peaks_intro), &ret.peaks, &ret.nr_peaks);
    }


    check_input(fp, "SSND");                                            size_to_read -= 4;
    assert(read_i32be(fp) == expected_size);    size_to_read -= 4;
    assert(read_i32be(fp) == 0);                   size_to_read -= 4;
    assert(read_i32be(fp) == 0);                   size_to_read -= 4;
    ret.samples = calloc(ret.nr_sample_points, sizeof(float));
    /*fread(ret.samples, sizeof(float), ret.nr_sample_points, fp);*/
    for(int i = 0; i < ret.nr_sample_points; i++) {
        ret.samples[i] =  read_f32be(fp);
        size_to_read -= 4;
    }
    assert(size_to_read == 0);
    return ret;
}


