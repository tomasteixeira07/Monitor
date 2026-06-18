# Browser Resource Monitor

A lightweight Linux system resource monitor for tracking browser memory and CPU usage.

<img width="673" height="341" alt="imagem" src="https://github.com/user-attachments/assets/a101897e-1151-4b4e-8cf9-ba62ac849349" />

## Features

- Real-time memory (Total, Available, Used) with a visual bar
- CPU usage monitoring with a visual bar
- Track multiple browsers simultaneously (Firefox, Chrome, Brave, etc.)
- Add custom browsers not in the default list
- Lightweight (~4.4 MB RAM)
- CPU: ~7% of one core

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

Select browsers to monitor (0-3), press '.' to exit. Optionally add custom ones. Then press 'q' to exit.

## Building

```bash
make          # Compile with optimizations
make clean    # Remove binary
```
