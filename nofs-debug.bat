openocd.exe ^
-c "tcl_port disabled" ^
-c "gdb_port 3333" ^
-c "telnet_port 4444" ^
-s B:\OpenOCD\share\openocd\scripts ^
-f A:\nofs\stlink.cfg ^
-c "program A:/nofs/build/nofs.elf" ^
-c "init;reset init;" ^
-c "echo  ===<- READY ->=== "
