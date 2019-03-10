#target extended-remote :3333
target extended-remote | openocd -f board/ek-tm4c123gxl.cfg -c "gdb_port pipe; log_output openocd.log"
load
# monitor reset init
# monitor resume
