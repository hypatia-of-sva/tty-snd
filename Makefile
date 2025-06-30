CC = gcc
CFLAGS = -Wall -Werror -g
#-std=c90
LDFLAGS =  -lm -L../tool_2/raylib/src/ -lraylib
#-lgsl -lgslcblas
IFLAGS = -I. -I../tool_2/raylib/src/


COMMON-SOURCES = util.c simple_wav.c f80.c

FFT-SOURCES = fft_main.c fft.c $(COMMON-SOURCES)
FFT-OBJECTS = $(FFT-SOURCES:.c=.o)
FFT-TARGET = tty-snd-fft


WAV-SOURCES = wav_main.c wav.c $(COMMON-SOURCES)
WAV-OBJECTS = $(WAV-SOURCES:.c=.o)
WAV-TARGET = tty-snd-wav



GRAPH-SOURCES = graph_main.c $(COMMON-SOURCES)
GRAPH-OBJECTS = $(GRAPH-SOURCES:.c=.o)
GRAPH-TARGET = tty-snd-graph

PEAK-SOURCES = cutoff_intervals.c peak_main.c $(COMMON-SOURCES)
PEAK-OBJECTS = $(PEAK-SOURCES:.c=.o)
PEAK-TARGET = tty-snd-peaks


.PHONY: all
all: $(WAV-TARGET) $(FFT-TARGET) $(GRAPH-TARGET) $(PEAK-TARGET)

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@  $^ $(IFLAGS)

#$(FORMANT-TARGET) : $(FORMANT-OBJECTS)
#	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(WAV-TARGET) : $(WAV-OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)


$(FFT-TARGET) : $(FFT-OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)


$(GRAPH-TARGET) : $(GRAPH-OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(PEAK-TARGET) : $(PEAK-OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
