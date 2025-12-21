# tty-snd

**tty-snd** (terminal sound, or \[tɪti saʊnd\]) is a modular system for creating, modifying and displaying audio streams using standard input/output pipes.

## Modules

| Module | Description | Arguments |
|--------|-------------|-----------|
| `tty-snd-mic-src` | Records audio from microphone | `device-id` `duration-seconds` |
| `tty-snd-graph` | Displays audio stream as graph | *(none)* |
| `tty-snd-fft` | Applies Fourier transform | `reduction-power` `index` `[-w window]` |
| `tty-snd-wav` | Loads WAV file as stream | `filename` `channel-nr` |
| `tty-snd-peaks` | Finds frequency peaks (formants) | `nr_of_steps` `min_cents_difference` |
| `tty-snd-bfilter` | Box filter (bandpass) | `min_freq` `max_freq` |
| `tty-snd-ifft` | Inverse Fourier transform | `reduction-power` `index` |

---

## Installation

### Windows (CMD only)

> ⚠️ **PowerShell corrupts binary pipes.** Use Command Prompt (`cmd.exe`) for all commands.

**Prerequisites:**
- CMake: `winget install Kitware.CMake`
- Visual Studio with "Desktop C++ workload"
- Git: `winget install Git.Git`

**Build:**
```cmd
git clone <repository-url>
cd tty-snd
cmake -S . -B build/win
cmake --build build/win -j %NUMBER_OF_PROCESSORS%

:: Copy required DLL (one-time)
copy build\win\_deps\openal_soft-build\Debug\OpenAL32.dll build\win\Debug\
```

### Linux

**Prerequisites:**
```bash
# Debian/Ubuntu
sudo apt install git cmake build-essential libopenal-dev libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libgl1-mesa-dev
```

**Build:**
```bash
git clone <repository-url>
cd tty-snd
cmake -S . -B build/linux
cmake --build build/linux -j $(nproc)
```

---

## Quickstart

### Windows

```cmd
:: List available microphones
build\win\Debug\tty-snd-mic-src.exe

:: Record 5 seconds from mic #0 and display graph
build\win\Debug\tty-snd-mic-src.exe 0 5.0 | build\win\Debug\tty-snd-graph.exe

:: Apply FFT then visualize
build\win\Debug\tty-snd-mic-src.exe 0 5.0 | build\win\Debug\tty-snd-fft.exe 0 0 | build\win\Debug\tty-snd-graph.exe
```

### Linux

```bash
# List available microphones
./build/linux/tty-snd-mic-src

# Record 5 seconds from mic #0 and display graph
./build/linux/tty-snd-mic-src 0 5.0 | ./build/linux/tty-snd-graph
```

### WSL Testing (No Mic Access)

If you're on WSL and the microphone doesn't work, generate audio on Windows first:

```cmd
:: On Windows CMD - generate test audio file
build\win\Debug\tty-snd-mic-src.exe 0 3.0 > test_audio.aiff
```

Then test Linux tools with that file:

```bash
# On WSL/Linux - test processing pipeline
./build/linux/tty-snd-fft 0 0 < test_audio.aiff > fft_out.aiff
./build/linux/tty-snd-bfilter 100 2000 < fft_out.aiff > filtered.aiff

# Note: Graph display has a known rendering issue on Linux (see ROADMAP.md)
```

---

## How It Works

The tools use Unix-style pipes to chain audio processing:

```
[mic-src] → raw audio → [fft] → frequency data → [peaks] → formants
                                      ↓
                                  [graph] → visualization
```

All tools communicate using a custom AIFF-based binary format with 32-bit floats.

---

## Troubleshooting

| Problem | Solution |
|---------|----------|
| `OpenAL32.dll not found` | Copy DLL: `copy build\win\_deps\openal_soft-build\Debug\OpenAL32.dll build\win\Debug\` |
| Assertion error on Windows | Use `cmd.exe` instead of PowerShell |
| No microphones listed | Check audio device drivers and permissions |
| Linux graph shows wrong data | Known issue - see [ROADMAP.md](ROADMAP.md) |

---

## Known Issues

See [ROADMAP.md](ROADMAP.md) for known bugs and planned improvements.

## License

See [LICENSE](LICENSE) file.
