#!/usr/bin/tclsh

set delay_ms 3000

if ![info exists ::env(start_halscope)] {
  set ::env(start_halscope) $::argv
  exec ./start_halscope.tcl & ;# restart this script with env(start_halscope) set
  exit 0
} else {
  after $delay_ms
  eval exec halscope -i a.halscope &
  exit 0
}

