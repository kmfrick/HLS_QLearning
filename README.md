# HLS_QLearning

Q-Learning on a ZedBoard with Vivado HLS. 

Project work for the [Digital Systems M](https://www.unibo.it/en/teaching/course-unit-catalogue/course-unit/2020/385380) course for the MSc Computer Engineering at the University of Bologna. 

## How to run

Tested with Vivado Design Suite 2019.2. Where settings are not specified, defaults are fine. 

1. Create a new Vivado HLS project, selecting the ZedBoard as a target in the "Boards" tab.
2. Add `HLS_CartPole/learn.c` and `HLS_CartPole/globals.h` as source files and `HLS_CartPole/main.c` as a test bench. If you run the C simulation, the output should be 210 periods.
3. Run 'C Synthesis' and 'Export RTL' in Vivado HLS.
4. Create a new Vivado project, selecting the ZedBoard as a target in the "Boards" tab (very important to avoid issues with location constraints).
5. Add the `HLS_CartPole` folder to the IP repository.
6. Add the `design_1.bd` block design file and the `constraints.xdc` constraints file to the project.
7. Run 'Validate design', then 'Create HDL wrapper' on `design_1.bd`.
8. Run 'Generate bitstream'.
9. Run 'Export hardware', checking 'Include bitstream'.
10. Create a new Vitis platform project from XSA file, selecting the `design_1_wrapper.xsa` file inside the Vivado project folder.
11. Build the platform project.
12. Create a new Vitis application project, selecting the platform project above as a target. 
13. Add the `Vitis_CartPole/src/learn.c` file as a source file. 
14. Build the application project. 
15. Connect the ZedBoard to the USB UART and USB JTAG ports. 
16. Turn on the ZedBoard.
17. Install and run `gtkterm` as superuser, connect to `/dev/ttyACM0`, baud rate 115200.
18. Right-click the application project, select 'Run -> Run on Hardware'. Output will be captured by `gtkterm` :)
