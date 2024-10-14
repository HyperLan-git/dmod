# DMod
A strange plugin that enables you to add harmonics to a signal through modulating a simple delay. It really sounds like frequency (or rather phase) modulation when used on single oscillators. With this, you can modulate anything ! It comes at a cost though : 50ms of delay on the input signal and there's no anti-aliasing (yet). Careful to not clip with the modulator...

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
