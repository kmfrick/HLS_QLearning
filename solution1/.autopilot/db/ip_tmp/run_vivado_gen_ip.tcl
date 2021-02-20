create_project prj -part xc7vx485t-ffg1157-1 -force
set_property target_language verilog [current_project]
set vivado_ver [version -short]
set COE_DIR "../../syn/verilog"
source "/home/kmfrick/Code/HLS_CartPole/solution1/syn/verilog/learn_ap_ddiv_20_no_dsp_64_ip.tcl"
if {[regexp -nocase {2015\.3.*} $vivado_ver match] || [regexp -nocase {2016\.1.*} $vivado_ver match]} {
    extract_files -base_dir "./prjsrcs/sources_1/ip" [get_files -all -of [get_ips learn_ap_ddiv_20_no_dsp_64]]
}
source "/home/kmfrick/Code/HLS_CartPole/solution1/syn/verilog/learn_ap_dadddsub_3_full_dsp_64_ip.tcl"
if {[regexp -nocase {2015\.3.*} $vivado_ver match] || [regexp -nocase {2016\.1.*} $vivado_ver match]} {
    extract_files -base_dir "./prjsrcs/sources_1/ip" [get_files -all -of [get_ips learn_ap_dadddsub_3_full_dsp_64]]
}
source "/home/kmfrick/Code/HLS_CartPole/solution1/syn/verilog/learn_ap_dadd_3_full_dsp_64_ip.tcl"
if {[regexp -nocase {2015\.3.*} $vivado_ver match] || [regexp -nocase {2016\.1.*} $vivado_ver match]} {
    extract_files -base_dir "./prjsrcs/sources_1/ip" [get_files -all -of [get_ips learn_ap_dadd_3_full_dsp_64]]
}
source "/home/kmfrick/Code/HLS_CartPole/solution1/syn/verilog/learn_ap_fmul_1_max_dsp_32_ip.tcl"
if {[regexp -nocase {2015\.3.*} $vivado_ver match] || [regexp -nocase {2016\.1.*} $vivado_ver match]} {
    extract_files -base_dir "./prjsrcs/sources_1/ip" [get_files -all -of [get_ips learn_ap_fmul_1_max_dsp_32]]
}
source "/home/kmfrick/Code/HLS_CartPole/solution1/syn/verilog/learn_ap_fadd_2_full_dsp_32_ip.tcl"
if {[regexp -nocase {2015\.3.*} $vivado_ver match] || [regexp -nocase {2016\.1.*} $vivado_ver match]} {
    extract_files -base_dir "./prjsrcs/sources_1/ip" [get_files -all -of [get_ips learn_ap_fadd_2_full_dsp_32]]
}
source "/home/kmfrick/Code/HLS_CartPole/solution1/syn/verilog/learn_ap_dmul_3_max_dsp_64_ip.tcl"
if {[regexp -nocase {2015\.3.*} $vivado_ver match] || [regexp -nocase {2016\.1.*} $vivado_ver match]} {
    extract_files -base_dir "./prjsrcs/sources_1/ip" [get_files -all -of [get_ips learn_ap_dmul_3_max_dsp_64]]
}
source "/home/kmfrick/Code/HLS_CartPole/solution1/syn/verilog/learn_ap_fptrunc_0_no_dsp_64_ip.tcl"
if {[regexp -nocase {2015\.3.*} $vivado_ver match] || [regexp -nocase {2016\.1.*} $vivado_ver match]} {
    extract_files -base_dir "./prjsrcs/sources_1/ip" [get_files -all -of [get_ips learn_ap_fptrunc_0_no_dsp_64]]
}
source "/home/kmfrick/Code/HLS_CartPole/solution1/syn/verilog/learn_ap_dcmp_0_no_dsp_64_ip.tcl"
if {[regexp -nocase {2015\.3.*} $vivado_ver match] || [regexp -nocase {2016\.1.*} $vivado_ver match]} {
    extract_files -base_dir "./prjsrcs/sources_1/ip" [get_files -all -of [get_ips learn_ap_dcmp_0_no_dsp_64]]
}
source "/home/kmfrick/Code/HLS_CartPole/solution1/syn/verilog/learn_ap_fpext_0_no_dsp_32_ip.tcl"
if {[regexp -nocase {2015\.3.*} $vivado_ver match] || [regexp -nocase {2016\.1.*} $vivado_ver match]} {
    extract_files -base_dir "./prjsrcs/sources_1/ip" [get_files -all -of [get_ips learn_ap_fpext_0_no_dsp_32]]
}
source "/home/kmfrick/Code/HLS_CartPole/solution1/syn/verilog/learn_ap_fcmp_0_no_dsp_32_ip.tcl"
if {[regexp -nocase {2015\.3.*} $vivado_ver match] || [regexp -nocase {2016\.1.*} $vivado_ver match]} {
    extract_files -base_dir "./prjsrcs/sources_1/ip" [get_files -all -of [get_ips learn_ap_fcmp_0_no_dsp_32]]
}
