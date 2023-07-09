cmake_minimum_required(VERSION 3.22)
set(BOARD xiao_esp32c3)

find_package(Zephyr)
project(digital_speedometer)

include(${ZEPHYR_BASE}/cmake/cfb.cmake NO_POLICY_SCOPE)

target_sources(app PRIVATE src/main.c src/nmea_parser.h src/nmea_parser.c)

set(gen_dir ${ZEPHYR_BINARY_DIR}/include/generated/)
generate_cfb_font_for_target(app fonts/JetBrainsMono-Bold.ttf ${gen_dir}/cfb_font_speed_bold.h 26 48 -s 40 --first 32 --last 126)
