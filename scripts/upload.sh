#!/usr/bin/env bash

pio run || exit 1

for port in /dev/tty.usbmodem*; do
    echo "==> $port"
    pio run -t nobuild -t upload --upload-port "$port"
done
