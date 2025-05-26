#!/bin/bash
# Terminal 1: OpenOCD server
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg

