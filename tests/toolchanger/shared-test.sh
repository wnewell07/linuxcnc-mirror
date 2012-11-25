#!/bin/bash
set -x

rm -f sim.var

# customize the ini file for the actual test directory
rm -f sim.ini
sed -e "s|@INI_DIR@|$PWD|g" sim.ini.in > sim.ini

# reset the tool table to a known starting configuration
# this has T1 in pocket 1 and T99999 in pocket 50
# (and nothing in the spindle)
rm -f simpockets.tbl
cp ../simpockets.tbl.orig simpockets.tbl

rm -f gcode-output

linuxcnc sim.ini &

# let linuxcnc come up
sleep 5

(
    echo hello EMC mt 1.0
    echo set enable EMCTOO

    echo set estop off
    echo set machine on

    echo set home 0
    echo set home 1
    echo set home 2
    sleep 1

    echo set mode mdi

    echo 'set mdi m100 P0 Q#5420'  # X
    echo 'set mdi m100 P1 Q#5421'  # Y
    echo 'set mdi m100 P2 Q#5422'  # Z
    echo 'set mdi m100 P3 Q#5400'  # toolno
    echo 'set mdi m100 P4 Q#5403'  # TLO z
    echo 'set mdi m100 P5'         # blank line
    sleep .25

    echo set mdi t1 m6
    echo 'set mdi m100 P0 Q#5420'  # X
    echo 'set mdi m100 P1 Q#5421'  # Y
    echo 'set mdi m100 P2 Q#5422'  # Z
    echo 'set mdi m100 P3 Q#5400'  # toolno
    echo 'set mdi m100 P4 Q#5403'  # TLO z
    echo 'set mdi m100 P5'         # blank line
    sleep .25

    echo set mdi g43
    echo 'set mdi m100 P0 Q#5420'  # X
    echo 'set mdi m100 P1 Q#5421'  # Y
    echo 'set mdi m100 P2 Q#5422'  # Z
    echo 'set mdi m100 P3 Q#5400'  # toolno
    echo 'set mdi m100 P4 Q#5403'  # TLO z
    echo 'set mdi m100 P5'         # blank line
    sleep .25

    echo set mdi g10 l10 p1 z.1
    echo 'set mdi m100 P0 Q#5420'  # X
    echo 'set mdi m100 P1 Q#5421'  # Y
    echo 'set mdi m100 P2 Q#5422'  # Z
    echo 'set mdi m100 P3 Q#5400'  # toolno
    echo 'set mdi m100 P4 Q#5403'  # TLO z
    echo 'set mdi m100 P5'         # blank line
    sleep .25

    echo set mdi g43
    echo 'set mdi m100 P0 Q#5420'  # X
    echo 'set mdi m100 P1 Q#5421'  # Y
    echo 'set mdi m100 P2 Q#5422'  # Z
    echo 'set mdi m100 P3 Q#5400'  # toolno
    echo 'set mdi m100 P4 Q#5403'  # TLO z
    echo 'set mdi m100 P5'         # blank line
    sleep .25

    echo set mdi g10 l10 p99999 z.2
    echo 'set mdi m100 P0 Q#5420'  # X
    echo 'set mdi m100 P1 Q#5421'  # Y
    echo 'set mdi m100 P2 Q#5422'  # Z
    echo 'set mdi m100 P3 Q#5400'  # toolno
    echo 'set mdi m100 P4 Q#5403'  # TLO z
    echo 'set mdi m100 P5'         # blank line
    sleep .25

    echo set mdi g43
    echo 'set mdi m100 P0 Q#5420'  # X
    echo 'set mdi m100 P1 Q#5421'  # Y
    echo 'set mdi m100 P2 Q#5422'  # Z
    echo 'set mdi m100 P3 Q#5400'  # toolno
    echo 'set mdi m100 P4 Q#5403'  # TLO z
    echo 'set mdi m100 P5'         # blank line
    sleep .25

    echo set mdi t99999 m6
    echo 'set mdi m100 P0 Q#5420'  # X
    echo 'set mdi m100 P1 Q#5421'  # Y
    echo 'set mdi m100 P2 Q#5422'  # Z
    echo 'set mdi m100 P3 Q#5400'  # toolno
    echo 'set mdi m100 P4 Q#5403'  # TLO z
    echo 'set mdi m100 P5'         # blank line
    sleep .25

    echo set mdi g43
    echo 'set mdi m100 P0 Q#5420'  # X
    echo 'set mdi m100 P1 Q#5421'  # Y
    echo 'set mdi m100 P2 Q#5422'  # Z
    echo 'set mdi m100 P3 Q#5400'  # toolno
    echo 'set mdi m100 P4 Q#5403'  # TLO z
    echo 'set mdi m100 P5'         # blank line
    sleep .25

    echo set mdi g10 l10 p1 z.3
    echo 'set mdi m100 P0 Q#5420'  # X
    echo 'set mdi m100 P1 Q#5421'  # Y
    echo 'set mdi m100 P2 Q#5422'  # Z
    echo 'set mdi m100 P3 Q#5400'  # toolno
    echo 'set mdi m100 P4 Q#5403'  # TLO z
    echo 'set mdi m100 P5'         # blank line
    sleep .25

    echo set mdi g43
    echo 'set mdi m100 P0 Q#5420'  # X
    echo 'set mdi m100 P1 Q#5421'  # Y
    echo 'set mdi m100 P2 Q#5422'  # Z
    echo 'set mdi m100 P3 Q#5400'  # toolno
    echo 'set mdi m100 P4 Q#5403'  # TLO z
    echo 'set mdi m100 P5'         # blank line
    sleep .25

    echo set mdi g10 l10 p99999 z.4
    echo 'set mdi m100 P0 Q#5420'  # X
    echo 'set mdi m100 P1 Q#5421'  # Y
    echo 'set mdi m100 P2 Q#5422'  # Z
    echo 'set mdi m100 P3 Q#5400'  # toolno
    echo 'set mdi m100 P4 Q#5403'  # TLO z
    echo 'set mdi m100 P5'         # blank line
    sleep .25

    echo set mdi g43
    echo 'set mdi m100 P0 Q#5420'  # X
    echo 'set mdi m100 P1 Q#5421'  # Y
    echo 'set mdi m100 P2 Q#5422'  # Z
    echo 'set mdi m100 P3 Q#5400'  # toolno
    echo 'set mdi m100 P4 Q#5403'  # TLO z
    echo 'set mdi m100 P5'         # blank line
    sleep .25

    # give linuxcnc a second to finish
    sleep 1.0

    echo shutdown
) | telnet localhost 5007


# wait for linuxcnc to finish
wait

exit 0

