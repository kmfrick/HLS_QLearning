############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2019 Xilinx, Inc. All Rights Reserved.
############################################################
open_project HLS_CartPole
set_top learn
add_files HLS_CartPole/globals.hpp
add_files HLS_CartPole/pole.cpp
add_files -tb HLS_CartPole/main.cpp -cflags "-Wno-unknown-pragmas"
open_solution "solution1"
set_part {xc7z020-clg484-1}
create_clock -period 10 -name default
config_sdx -target none
config_export -format ip_catalog -rtl vhdl -vivado_optimization_level 2 -vivado_phys_opt place -vivado_report_level 0
set_clock_uncertainty 12.5%
source "./HLS_CartPole/solution1/directives.tcl"
csim_design -clean
csynth_design
cosim_design -trace_level all -rtl vhdl
export_design -rtl vhdl -format ip_catalog
