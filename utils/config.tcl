open_hw_manager
connect_hw_server -allow_non_jtag
open_hw_target
current_hw_device [get_hw_devices xcu50_0]
refresh_hw_device -update_hw_probes false [lindex [get_hw_devices xcu50_0] 0]
set_property PROGRAM.FILE $::env(BITFILE) [get_hw_devices xcu50_0]
program_hw_device [get_hw_devices xcu50_0]
disconnect_hw_server localhost:3121
close_hw_manager

