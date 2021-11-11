# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(nrf52-pwm-with-opus-player)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} \
")

target_sources(app PRIVATE
  lib/sbc-1.5/sbc/sbc_primitives.c
  lib/sbc-1.5/sbc/sbc.c

  src/nrf_pwm_audio.c
  src/main.c
)

zephyr_include_directories(
  lib/sbc-1.5/sbc
  src
)
