# Browser Resource Monitor

A lightweight Linux system resource monitor for tracking browser memory and CPU usage.

## Features

- Real-time memory (Total, Available, Used) with a visual bar
- CPU usage monitoring with a visual bar
- Track multiple browsers simultaneously (Firefox, Chrome, Brave, etc.)
- Add custom browsers not in the default list
- Lightweight (~4.7 MB RAM)
- CPU: ~7% of one core
- Configurable precision (PIDs cleanup interval)

## Installation

```bash
git clone https://github.com/tomasteixeira07/Monitor.git
cd Monitor
make
./monitor
```

## Requirements

- Linux with `/proc` filesystem
- ncurses development libraries
- C++11 or later

```bash
sudo apt install libncurses-dev g++
```

## Usage

```bash
./monitor
```

Select browsers to monitor (0-7), press '.' to exit. Optionally add custom ones. Set precision interval, then press 'q' to exit.

## Performance

- RAM: ~4.7 MB
- CPU: <5% (varies with system load)
- I/O: Minimal (only reads `/proc`)

## Building

```bash
make          # Compile with optimizations
make clean    # Remove binary
```

## License

MIT License