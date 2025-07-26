#include "common.h"
#include <unistd.h>


void alinfo(void) {
        printf("General AL info:\n");
        const char* vendor = alGetString(AL_VENDOR);
        const char* ver = alGetString(AL_VERSION);
        const char* rend = alGetString(AL_RENDERER);
        const char* exts = alGetString(AL_EXTENSIONS);
        printf("Vendor: %s\nVersion: %s\nRenderer: %s\nExtensions: %s\n", vendor,ver, rend, exts);

}

int main(int argc, char** argv) {
    aladLoadAL();

    if(argc < 2) {
        printf("Available output devices:\n");
        const char* capture_dev_string = alcGetString(NULL, ALC_DEVICE_SPECIFIER);

        int num;
        char** devs = split(capture_dev_string, strlen(capture_dev_string), '\0', &num);

        for(int i = 0; i < num; i++) {
            printf("%i: %s\n", i, devs[i]);
            free(devs[i]);
        }

        free(devs);

        const char* default_dev = alcGetString(NULL,ALC_DEFAULT_DEVICE_SPECIFIER);
        printf("default: %s\n", default_dev);

    } else {
        simple_wav_t float_form = read_simple_wav(stdin);

        int dev_nr = atoi(argv[1]);
        assert(dev_nr >= 0);

        const char* capture_dev_string = alcGetString(NULL, ALC_DEVICE_SPECIFIER);

        int num;
        char** devs = split(capture_dev_string, strlen(capture_dev_string), '\0', &num);
        assert(dev_nr < num);


        fprintf(stderr, "chosen dev: %s\n", devs[dev_nr]);

        ALCdevice* output = alcOpenDevice(devs[dev_nr]);

        for(int i = 0; i < num; i++) {
            free(devs[i]);
        }

        free(devs);


        int code = alcGetError(output);
        if(code != ALC_NO_ERROR)  {
            fprintf(stderr,"error %i\n", code);
            exit(-1);
        }

        if(output == NULL) die("no output found");

        int new_len = float_form.nr_sample_points/2;

        float* float_buffer = calloc(sizeof(float),new_len);
        for(int i = 0; i < new_len; i++) {
            float_buffer[i] = sqrt(float_form.samples[2*i]*float_form.samples[2*i]
                            + float_form.samples[2*i+1]*float_form.samples[2*i+1]);
        }

        float cur_max = 0.0f;
        for(int i = 0; i < new_len; i++) {
            if(fabs(float_buffer[i]) > cur_max)
                cur_max = fabs(float_buffer[i]);
        }

        int16_t* int_buffer = calloc(sizeof(int16_t),new_len);
        for(int i = 0; i < new_len; i++) {
            int_buffer[i] = floor((float_buffer[i]/cur_max)*INT16_MAX);
        }

        ALuint bufferid, sourceid;
        alGenBuffers(1, &bufferid);

        alBufferData(bufferid, AL_FORMAT_MONO16, int_buffer,  new_len, floor(float_form.frequency_in_hz));


        alGenSources(1, &sourceid);

        alSourcei(sourceid, AL_BUFFER, bufferid);
        alSourcePlay(sourceid);


        assert(usleep((new_len/float_form.frequency_in_hz)*1000000.0f) == 0);
        while(true) {
            assert(usleep(1000.0) == 0);
            int code;
            alGetSourcei(sourceid, AL_SOURCE_STATE, &code);
            if(code != AL_PLAYING) break;
        }

        alDeleteSources(1, &sourceid);
        alDeleteBuffers(1, &bufferid);

        alcCloseDevice(output);
    }

    aladTerminate();

    return 0;
}
