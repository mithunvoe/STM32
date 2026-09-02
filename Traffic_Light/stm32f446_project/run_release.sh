#!/bin/bash
#build
cargo build --release
#flash
openocd \
  -f interface/stlink.cfg \
  -f target/stm32f4x.cfg \
  -c "program target/thumbv7em-none-eabihf/release/stm32f446_project verify reset exit"
