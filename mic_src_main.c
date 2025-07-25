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

    if(argc < 3) {
        printf("Available mics:\n");
        const char* capture_dev_string = alcGetString(NULL, ALC_CAPTURE_DEVICE_SPECIFIER);

        int num;
        char** devs = split(capture_dev_string, strlen(capture_dev_string), '\0', &num);

        for(int i = 0; i < num; i++) {
            printf("%i: %s\n", i, devs[i]);
            free(devs[i]);
        }

        free(devs);

        const char* default_dev = alcGetString(NULL,ALC_CAPTURE_DEFAULT_DEVICE_SPECIFIER);
        printf("default: %s\n", default_dev);

    } else {

        int dev_nr = atoi(argv[1]);
        assert(dev_nr >= 0);

        const char* capture_dev_string = alcGetString(NULL, ALC_CAPTURE_DEVICE_SPECIFIER);

        int num;
        char** devs = split(capture_dev_string, strlen(capture_dev_string), '\0', &num);
        assert(dev_nr < num);


        float time = atof(argv[2]);

        size_t buffersize = 44100*time;

        fprintf(stderr, "chosen dev: %s\n", devs[dev_nr]);

        ALCdevice* capture = alcCaptureOpenDevice(devs[dev_nr], 44100, AL_FORMAT_STEREO16, buffersize*100);

        for(int i = 0; i < num; i++) {
            free(devs[i]);
        }

        free(devs);

        int code = alcGetError(capture);
        if(code != ALC_NO_ERROR)  {
            fprintf(stderr,"error %i\n", code);
            exit(-1);
        }

        if(capture == NULL) die("no mic found");



        alcCaptureStart(capture);
        int16_t* buf = calloc(sizeof(int16_t), buffersize*10);

        code = alcGetError(capture);
        if(code != ALC_NO_ERROR)  {
            fprintf(stderr,"error %i\n", code);
            exit(-1);
        }

        assert(usleep(time*1000000) == 0);

        for(int i = 0;;i++) {

            fprintf(stderr,"iteration %i\n", i);

            int currently_recorded_samples = 0;
            while(currently_recorded_samples < buffersize) {
                alcGetIntegerv(capture, ALC_CAPTURE_SAMPLES, sizeof(int), &currently_recorded_samples);
                fprintf(stderr,"size: %i\n", currently_recorded_samples);
            }


            alcCaptureSamples(capture, buf, buffersize);

            int code = alcGetError(capture);
            if(code != ALC_NO_ERROR)  {
                fprintf(stderr,"error %i\n", code);
                break;
            }

            bool is_all_0 = true;
            for(int i = 0; i < buffersize; i++) {
                if(buf[i] != 0) {
                    is_all_0 = false;
                    break;
                }
            }
            if(!is_all_0) break;

        }
        alcCaptureStop(capture);


        size_t len = truncate_power_of_2(buffersize);
        float freq = ((float)44100);
        if(!is_power_of_2(len)) die("Data size collected not power of two!");

        float* amplitudes = transform_to_complex_array(buf, len);


        simple_wav_t float_form;
        float_form.frequency_in_hz = freq;
        float_form.nr_sample_points = 2*len;
        float_form.samples = amplitudes;

        write_simple_wav(stdout, float_form);

        alcCaptureCloseDevice(capture);
    }

    aladTerminate();

    return 0;
}
