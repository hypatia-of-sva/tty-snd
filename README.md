# tty-snd
tty-snd (terminal sound, or \[t​ɪti s​a​ʊnd\]) is a modular system for creating, modifying and displaying audio streams in standard input/output on the terminal

Current modules:

name | description | arguments
--- | --- | ---
tty-snd-wav | loads a wave file channel stream as complex floats | filename channel-nr
tty-snd-fft | transforms a stream into its Fourier-transform | reduction-power-of-two index \[-w window-param\]
tty-snd-graph | displays a stream in a raylib-graph-window | \[none\]
tty-snd-peaks | finds the peaks in a stream and displays as if they were frequency spikes (formants) | \[none\]
tty-mic-src | records audio from a microphone as complex floats to stdout | microphone-id recording-time

## How to run tty-snd on your maschine
First, get a current copy of the source code. The source code can be found on Github under https://github.com/hypatia-of-sva/tty-snd where you are most likely reading this right now.
To get a copy assuming you have git installed, navigate to a folder you would like your copy of the project to be stored in. Then run `$ git clone https://github.com/hypatia-of-sva/tty-snd`. If you do not have git installed please use githubs "code->Download Zip" function to download a zip file instead. Then extract the zip into a folder of your choosing. 
After this you should have a folder named TTY-SND. In this folder you will find the contents of the git repository. More specifically it's master branch.

The project itself depends upon raylib. A C/C++ library for drawing 2d graphics. Please make sure your system has raylib installed.
Additionally in order to easily build the project on your system using the build system management tool make is recommended. Make will be required on your system for the rest of this guide to work. 

After ensuring all dependencies are meet, navigate to the TTY-SND folder from github. If make is correctly installed on your system you should be able to compile the project by simply running `make`
Make now executes a bunch of gcc commands. One for each module of the project. 

Now you should be able to run the modules. To verify that the project compilled correctly, we recommend you run `$ ./tty-snd-mic-src`
you should now see a list of microphones connected to the system. On my system for example the command returns:

    Available mics:
    0: QUAD-CAPTURE 1-2 (1L+2R)
    default: QUAD-CAPTURE 1-2 (1L+2R)

Note the numbers in front of each interface aswell as the default: in front of one of them.
The microphone you would like to use must be specified ny passing its number as a console parameter to tty-snd-mic-src, in addition to the microphone to use the length of the desired recording in seconds must be specified. For example `$ ./tty-snd-src 0 10.5` will record sound from microphone 0 for 10.5 seconds and output it to the standard output stream. By default the standard output stream is displayed in your console window. That does not help us much here as a text based terminal console can not play sound. To actually listen to the output you need to give it to use a program that can playback raw pcm. The easiest way to do so is to pipe it into a file for later use. To pipe the output into a file use the pipe operator `>` like so `$ ./tty-snd-src 0 10.5 > soundtest.wav`. The wav file can now be played back with a program of your choice. The author of this redme recommends using ffplay from the ffmpeg converter / transcoder package. An example playback command might look like so `$ ffplay soundtest.wav`. If you hear a bunch of noise that is nothing like what you recorded, **do not panic**. The program is working correctly and the reason for you hearing a bunch of noise is an internal format transformation the projects input tools perform so the processing tools can work with the data from your mic.