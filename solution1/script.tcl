############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2019 Xilinx, Inc. All Rights Reserved.
############################################################
open_project HLS_CartPole
set_top learn
add_files HLS_CartPole/pole.c
add_files HLS_CartPole/globals.h
add_files -tb HLS_CartPole/main.c -cflags "-lm -Wno-unknown-pragmas" -csimflags "-Wno-unknown-pragmas"
open_solution "solution1"
set_part {xc7vx485t-ffg1157-1}
create_clock -period 10 -name default
config_export -format ip_catalog -rtl verilog
#source "./HLS_CartPole/solution1/directives.tcl"
csim_design -clean
csynth_design
cosim_design
export_design -rtl verilog -format ip_catalog
