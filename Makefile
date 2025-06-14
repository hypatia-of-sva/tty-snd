CC = gcc
CFLAGS = -Wall -Werror -g
#-std=c90
LDFLAGS =  -lm -L../tool_2/raylib/src/ -lraylib
#-lgsl -lgslcblas
IFLAGS = -I. -I../tool_2/raylib/src/



FFT-SOURCES = fft_main.c fft.c util.c simple_wav.c
FFT-OBJECTS = $(FFT-SOURCES:.c=.o)
FFT-TARGET = tty-snd-fft


WAV-SOURCES = wav_main.c wav.c util.c simple_wav.c
WAV-OBJECTS = $(WAV-SOURCES:.c=.o)
WAV-TARGET = tty-snd-wav



GRAPH-SOURCES = graph_main.c util.c simple_wav.c
GRAPH-OBJECTS = $(GRAPH-SOURCES:.c=.o)
GRAPH-TARGET = tty-snd-graph

#FORMANT-SOURCES = cutoff_intervals.c formant_main.c
#FORMANT-OBJECTS = $(FORMANT-SOURCES:.c=.o)
#FORMANT-TARGET = formant-finder


.PHONY: all
all: $(WAV-TARGET) $(FFT-TARGET) $(GRAPH-TARGET)

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

