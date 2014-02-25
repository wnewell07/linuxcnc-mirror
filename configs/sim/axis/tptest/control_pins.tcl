#!/usr/bin/tclsh

set ::delay_ms 3000      ;# wait for halfiles
set ::enable_sim_pins 1  ;# make simple gui for ini pins

#----------------------------------------------------------------------
proc get_pin_list {} {
  set sim_pin_list [list \
                    ini.traj_arc_blend_enable \
                    ini.traj_arc_blend_fallback_enable \
                    ini.traj_arc_blend_gap_cycles \
                    ini.traj_arc_blend_optimization_depth \
                    ini.traj_arc_blend_ramp_freq \
                   ]
  foreach idx {0 1 2} {
    lappend sim_pin_list \
            ini.$idx.max_velocity \
            ini.$idx.max_acceleration
  }
  return $sim_pin_list
} ;# get_pin_list

proc show_all {} {
  package require Hal
  puts "ini hal pins:"
  puts [hal show pin ini]
} ;# show_all

proc connect_ini_pins_to_pyvcp {} {
  # NOTE:
  # When a hal input pin with a value (like ini.* pins)
  # is initially connected  to a signal, the pin
  # is immediately set to zero (float, bit, whatever)
  #
  # For this setup, we want to connect two inputs
  # (a ini.* pin and a pyvcp display for it) to
  # sim_pin which is not a conventional writer -- it
  # just uses setp or sets
  #
  # Method:
  #  1) read the initial pin value
  #  2) connect pin to a signal (with other readers)
  #     (pin value is reset to zero)
  #  3) restore the saved initial value
  # (This can't be done readily with a postgui halfile)
  #
  package require Hal
  foreach {pinname pyvcppinname signalname} \
          {ini.traj_arc_blend_enable             blend_enable          i:blend_enable
           ini.traj_arc_blend_fallback_enable    blend_fallback_enable i:blend_fallback_enable
           ini.traj_arc_blend_optimization_depth optimization_depth    i:blend_optimization_depth
           ini.traj_arc_blend_ramp_freq          ramp_freq             i:blend_ramp_freq
           ini.traj_arc_blend_gap_cycles         gap_cycles            i:blend_gap_cycles
          } {
     set ivalue [hal getp $pinname]
     # the pyvcp pins are optional
     if [catch {hal net $signalname pyvcp.$pyvcppinname} msg] {
       puts "<$msg> continuing"
     } else {
       # make signal only if pyvcp pin exists
       hal net $signalname $pinname 
       hal sets $signalname $ivalue
     }
   }

} ;# connect_ini_pins_to_pyvcp

#begin-----------------------------------------------------------------
if ![info exists ::env(control_pins)] {
  set ::env(control_pins) $::argv
  exec ./control_pins.tcl & ;# restart this script with env(control_pins) set
  exit 0
} else {
  after $::delay_ms
  #show_all
  connect_ini_pins_to_pyvcp
  if $::enable_sim_pins {
    if [catch {
      eval exec sim_pin [get_pin_list] &
    } msg ] {
      puts "$::argv0: sim_pin failed:"
      puts "  $msg"
    }
  }
  exit 0
}
