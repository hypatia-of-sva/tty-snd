./tty-snd-mic-src 0 5 > new_test_raw.aiff
cat new_test_raw.aiff | ./tty-snd-reduce > new_test.aiff
cat new_test_raw.aiff | ./tty-snd-fft 0 0 | ./tty-snd-stretch 1.0 | ./tty-snd-ifft 0 0 | ./tty-snd-reduce > new_test_hl.aiff
ffplay new_test.aiff
read
ffplay new_test_hl.aiff
