vsim work.latch_74LS373
force -freeze sim:/latch_74LS373/LE 0 0
force -freeze sim:/latch_74LS373/nOE 1 0
force -freeze sim:/latch_74LS373/D x\"88\" 0
add wave \
{sim:/latch_74LS373/D } \
{sim:/latch_74LS373/LE } \
{sim:/latch_74LS373/nOE } \
{sim:/latch_74LS373/Q } 
run
run
run
run
force -freeze sim:/latch_74LS373/LE 1 0
run
force -freeze sim:/latch_74LS373/LE 0 0
run
run
run
force -freeze sim:/latch_74LS373/nOE 0 0
run
run
force -freeze sim:/latch_74LS373/nOE 1 0
run
force -freeze sim:/latch_74LS373/D 10001001 0
force -freeze sim:/latch_74LS373/LE 1 0
force -freeze sim:/latch_74LS373/nOE 1 0
run
force -freeze sim:/latch_74LS373/LE 0 0
run
force -freeze sim:/latch_74LS373/D 10001000 0
run
run
force -freeze sim:/latch_74LS373/LE 1 0
force -freeze sim:/latch_74LS373/nOE 0 0
run
force -freeze sim:/latch_74LS373/LE 0 0
run
run
force -freeze sim:/latch_74LS373/nOE 1 0
