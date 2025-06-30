# tty-snd
tty-snd (terminal sound, or \[t​ɪti s​a​ʊnd\]) is a modular system for creating, modifying and displaying audio streams in standard input/output on the terminal

Current modules:

name | description | arguments
--- | --- | ---
tty-snd-wav | loads a wave file | filename channel-nr
tty-snd-fft | transforms a stream into its Fourier-transform | \[none\]
tty-snd-graph | displays a stream in a raylib-graph-window | \[none\]
tty-snd-peaks | finds the peaks in a stream and displays as if they were frequency spikes (formants) | \[none\]
