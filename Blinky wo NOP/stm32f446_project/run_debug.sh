#!/bin/bash

#build
cargo build

#flash

openocd   -f interface/stlink.cfg   -f target/stm32f4x.cfg   -c "program target/thumbv7em-none-eabihf/debug/stm32f446_project verify reset exit"

