# Roadmap

## Known Issues

### Critical
- **Audio capture sample rate mismatch** - Raw PCM output from `tty-snd-mic-src` plays back at wrong speed with buzz/noise. Sample rate or channel configuration in OpenAL capture is incorrect.
- **Linux graph rendering incorrect** - The AIFF header parsing in `simple_wav.c` reads wrong sample count on Linux (1M vs 131K on Windows). Likely endianness bug in `read_u32be()` or `read_i32be()`.

### High
- **tty-snd-peaks uninitialized variable** - `min_value` in `peak_main.c` is used without initialization, causing runtime failures on Windows.

### Medium
- **tty-snd-wav requires actual WAV file** - Not documented, crashes if file doesn't exist.
- **WSL audio capture not functional** - "RDP Source" virtual mic doesn't provide real audio data.

## Completed (This PR)
- [x] Windows build support via CMake/FetchContent
- [x] Cross-platform binary pipe handling (SET_BINARY_MODE)
- [x] Windows/Raylib symbol conflict resolution
- [x] Platform-specific library linking
- [x] Separate build directories (build/win, build/linux)
- [x] Updated README with Windows/Linux quickstarts

## Future Work
- [ ] Fix Linux AIFF header parsing
- [ ] Fix uninitialized variable in peak_main.c
- [ ] Add input validation to tty-snd-wav
- [ ] Add CI/CD for cross-platform testing
- [ ] Consider replacing AIFF format with simpler binary format
- [ ] Add unit tests for binary I/O functions
