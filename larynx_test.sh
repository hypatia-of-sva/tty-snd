./tty-snd-mic-src 0 10 | ./tty-snd-reduce > new_test.aiff
cat new_test.aiff | ./tty-snd-fft 0 0 | ./tty-snd-stretch 2.0 | ./tty-snd-ifft 0 0 | ./tty-snd-reduce > new_test_hl.aiff
ffplay new_test.aiff
read
ffplay new_test_hl.aiff
