vsim work.transceiver_8286
add wave \
{sim:/transceiver_8286/A } \
{sim:/transceiver_8286/B } \
{sim:/transceiver_8286/CD } \
{sim:/transceiver_8286/T_nR } 
force -freeze sim:/transceiver_8286/CD 1 0
force -freeze sim:/transceiver_8286/T_nR 1 0
run
run
force -freeze sim:/transceiver_8286/A x\"88\" 0
force -freeze sim:/transceiver_8286/CD 0 0
run
run
force -freeze sim:/transceiver_8286/CD 1 0
noforce sim:/transceiver_8286/A
run
force -freeze sim:/transceiver_8286/T_nR 0 0
force -freeze sim:/transceiver_8286/B x\"55\" 0
run
run
force -freeze sim:/transceiver_8286/CD 0 0
run
force -freeze sim:/transceiver_8286/CD 1 0
run
noforce sim:/transceiver_8286/B
run

