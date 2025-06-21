#include "common.h"


/* asserted fgetc */
char afgetc(FILE* fd) {
    char c = fgetc(fd);
    assert(c != EOF);
    return c;
}
/* asserted fputc */
void afputc(char c, FILE* fd) {
    assert(fputc(c, fd) != EOF);
}
uint16_t read_u16be(FILE* fd)  {
    return ((uint16_t)afgetc(fd)<<8) + ((uint16_t)afgetc(fd));
}
uint32_t read_u32be(FILE* fd)  {
    return ((uint32_t)afgetc(fd)<<24) + ((uint32_t)afgetc(fd)<<16) + ((uint32_t)afgetc(fd)<<8) + ((uint32_t)afgetc(fd));
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






const char* appdata = "VER: 0";

void write_simple_wav(FILE* fp, simple_wav_t data) {
    /* basic AIFF header */
    fprintf(fp, "FORM");
    write_i32be(fp, 84 + sizeof(appdata) + sizeof(float)*data.nr_sample_points);
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
    char extended[10];
    convert_to_extended_float_be((double) data.frequency_in_hz, &extended[0]);
    fwrite(&extended[0], 10, 1, fp);
    fprintf(fp, "fl32");
    afputc(0, fp);
    afputc(0, fp);
    fprintf(fp, "APPL");
    write_i32be(fp, sizeof(appdata)+12);
    fprintf(fp, "stoc");
    fprintf(fp, "\7tty-snd"); /* application id */
    fwrite(appdata, 1, sizeof(appdata), fp);
    fprintf(fp, "SSND");
    write_i32be(fp, sizeof(float)*data.nr_sample_points+8);
    write_i32be(fp, 0);
    write_i32be(fp, 0);
    /*fwrite(data.samples, sizeof(float), data.nr_sample_points, fp); */
    for(int i = 0; i < data.nr_sample_points; i++) {
        write_f32be(fp, data.samples[i]);
    }
}

simple_wav_t read_simple_wav(FILE* fp) {
    simple_wav_t ret;
    int32_t size_to_read;

    check_input(fp, "FORM");
    size_to_read = read_i32be(fp);
    assert(size_to_read > 0);
    check_input(fp, "AIFC");                        size_to_read -= 4;
    check_input(fp, "FVER");                        size_to_read -= 4;
    assert(read_i32be(fp) == 4);                    size_to_read -= 4;
    assert(afgetc(fp) == (char) 0xA2);              size_to_read -= 1;
    assert(afgetc(fp) == (char) 0x80);              size_to_read -= 1;
    assert(afgetc(fp) == (char) 0x51);              size_to_read -= 1;
    assert(afgetc(fp) == (char) 0x40);              size_to_read -= 1;
    check_input(fp, "COMM");                        size_to_read -= 4;
    assert(read_i32be(fp) == 24);                   size_to_read -= 4;
    assert(read_i16be(fp) == 1);                    size_to_read -= 2;
    ret.nr_sample_points = read_i32be(fp);          size_to_read -= 4;
    assert(read_i16be(fp) == 32);                   size_to_read -= 2;
    char extended[10];
    fread(&extended[0], 10, 1, fp);                 size_to_read -= 10;
    ret.frequency_in_hz = (float) convert_from_extended_float_be(&extended[0]);
    check_input(fp, "fl32");                        size_to_read -= 4;
    assert(afgetc(fp) == (char) 0);                 size_to_read -= 1;
    assert(afgetc(fp) == (char) 0);                 size_to_read -= 1;
    check_input(fp, "APPL");                        size_to_read -= 4;
    assert(read_i32be(fp) == sizeof(appdata)+12);   size_to_read -= 4;
    check_input(fp, "stoc");                        size_to_read -= 4;
    check_input(fp, "\7tty-snd");                   size_to_read -= 8;
    char* appdata_buf = malloc(sizeof(appdata));
    fread(appdata_buf, 1, sizeof(appdata), fp);         size_to_read -= sizeof(appdata);
    assert(strcmp(appdata, appdata_buf) == 0);
    check_input(fp, "SSND");                                            size_to_read -= 4;
    assert(read_i32be(fp) == sizeof(float)*ret.nr_sample_points+8);    size_to_read -= 4;
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


