CC = gcc
CFLAGS = -Wall -Werror -g
#-std=c90
RAYLIB_FOLDER = ../../tool_2/raylib
LDFLAGS =  -lm -L../../tool_2/raylib/src/ -lraylib
#-lgsl -lgslcblas
IFLAGS = -I. -I../../tool_2/raylib/src/


COMMON-SOURCES = util.c simple_wav.c f80.c alad.c

FFT-SOURCES = fft_main.c fft.c $(COMMON-SOURCES)
FFT-OBJECTS = $(FFT-SOURCES:.c=.o)
FFT-TARGET = tty-snd-fft

IFFT-SOURCES = ifft_main.c fft.c $(COMMON-SOURCES)
IFFT-OBJECTS = $(IFFT-SOURCES:.c=.o)
IFFT-TARGET = tty-snd-ifft


WAV-SOURCES = wav_main.c wav.c $(COMMON-SOURCES)
WAV-OBJECTS = $(WAV-SOURCES:.c=.o)
WAV-TARGET = tty-snd-wav



GRAPH-SOURCES = graph_main.c $(COMMON-SOURCES)
GRAPH-OBJECTS = $(GRAPH-SOURCES:.c=.o)
GRAPH-TARGET = tty-snd-graph

PEAK-SOURCES = cutoff_intervals.c peak_main.c $(COMMON-SOURCES)
PEAK-OBJECTS = $(PEAK-SOURCES:.c=.o)
PEAK-TARGET = tty-snd-peaks


MIC-SRC-SOURCES = mic_src_main.c $(COMMON-SOURCES)
MIC-SRC-OBJECTS = $(MIC-SRC-SOURCES:.c=.o)
MIC-SRC-TARGET = tty-snd-mic-src


STRETCH-SRC-SOURCES = stretch_main.c $(COMMON-SOURCES)
STRETCH-SRC-OBJECTS = $(STRETCH-SRC-SOURCES:.c=.o)
STRETCH-SRC-TARGET = tty-snd-stretch


PLAY-SOURCES = play_main.c $(COMMON-SOURCES)
PLAY-OBJECTS = $(PLAY-SOURCES:.c=.o)
PLAY-TARGET = tty-snd-play


REDUCE-SOURCES = reduce_main.c $(COMMON-SOURCES)
REDUCE-OBJECTS = $(REDUCE-SOURCES:.c=.o)
REDUCE-TARGET = tty-snd-reduce


PEAK-DBG-SOURCES = peak_dbg_main.c $(COMMON-SOURCES)
PEAK-DBG-OBJECTS = $(PEAK-DBG-SOURCES:.c=.o)
PEAK-DBG-TARGET = tty-snd-peak-print


CHANGE_ROLLOFF_VELOCITY-SOURCES = change_rolloff_v_main.c $(COMMON-SOURCES)
CHANGE_ROLLOFF_VELOCITY-OBJECTS = $(CHANGE_ROLLOFF_VELOCITY-SOURCES:.c=.o)
CHANGE_ROLLOFF_VELOCITY-TARGET = tty-snd-change_rolloff_velocity


.PHONY: all
all: $(WAV-TARGET) $(FFT-TARGET) $(GRAPH-TARGET) $(PEAK-TARGET) $(MIC-SRC-TARGET) $(STRETCH-SRC-TARGET) $(IFFT-TARGET) $(PLAY-TARGET) $(REDUCE-TARGET) $(PEAK-DBG-TARGET) $(CHANGE_ROLLOFF_VELOCITY-TARGET)

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@  $^ $(IFLAGS)

#$(FORMANT-TARGET) : $(FORMANT-OBJECTS)
#	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(WAV-TARGET) : $(WAV-OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)


$(FFT-TARGET) : $(FFT-OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(IFFT-TARGET) : $(IFFT-OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)


$(GRAPH-TARGET) : $(GRAPH-OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(PEAK-TARGET) : $(PEAK-OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)


$(MIC-SRC-TARGET) : $(MIC-SRC-OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)


$(STRETCH-SRC-TARGET) : $(STRETCH-SRC-OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)


$(PLAY-TARGET) : $(PLAY-OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)


$(REDUCE-TARGET) : $(REDUCE-OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)


$(PEAK-DBG-TARGET) : $(PEAK-DBG-OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(CHANGE_ROLLOFF_VELOCITY-TARGET) : $(CHANGE_ROLLOFF_VELOCITY-OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)



