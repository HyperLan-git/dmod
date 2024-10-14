# HSynth
A synthesizer that can take external input and operate on it as if it was an oscillator. I want to push my understanding of dsp and maths in general to its limits and implement approximations of operators that are as precise as possible.

## Dependencies
On windows, you just need Visual Studio 2022 and Juce.
In addition to juce's main dependencies on linux, you'll need libasan to compile in debug mode.

## How to compile
On windows, simply open the project from projucer and build the solution you need.

On linux, use the makefile :
```sh
make
```

To compile in Release mode (with optimisations and no memory sanitizer), use `make CONFIG=Release`.
You can clean binaries with `make clean`.
