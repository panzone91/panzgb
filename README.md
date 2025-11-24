# panzgb

panzgb is an emulator for the Gameboy system.

## Build

The PC client requires SDL2 installed on the system. Then panzgb uses cmake for building
```
mkdir build && cd build
cmake ..
make
```

## Run

```
./panzgb ROM-FILE
```

## What is working

- Basic support for running games
- Support for MBC1 and MBC3 games

Most notably, the sound system is currently not working. Also, this is a pure Gameboy emulator and it doesn't support Gameboy Color games.
