#!/usr/bin/env python2

import linuxcnc
import hal
import time
import sys
import os


# this is how long we wait for linuxcnc to do our bidding
timeout = 1.0


class LinuxcncError(Exception):
    pass
#    def __init__(self, value):
#        self.value = value
#    def __str__(self):
#        return repr(self.value)

class LinuxcncControl:
    '''
    issue G-Code commands
    make sure important modes are saved and restored
    mode is saved only once, and can be restored only once
    
    usage example: 
        e = emc_control()
        e.prepare_for_mdi()
            any internal sub using e.g("G0.....")
        e.finish_mdi()
    
    '''

    def __init__(self):
        self.c = linuxcnc.command()
        self.e = linuxcnc.error_channel()
        self.s = linuxcnc.stat()
        
    def running(self, do_poll=True):
        '''
        check wether interpreter is running.
        If so, cant switch to MDI mode.
        '''
        if do_poll: 
            self.s.poll()
        return (self.s.task_mode == linuxcnc.MODE_AUTO and 
                self.s.interp_state != linuxcnc.INTERP_IDLE)
                
    def set_mode(self,m):
        '''
        set EMC mode if possible, else throw LinuxcncError
        return current mode
        '''
        self.s.poll()
        if self.s.task_mode == m : 
            return m
        if self.running(do_poll=False): 
            raise LinuxcncError("interpreter running - cant change mode")
        self.c.mode(m)   
        self.c.wait_complete()
        return m  

    def set_state(self,m):
        '''
        set EMC mode if possible, else throw LinuxcncError
        return current mode
        '''
        self.s.poll()
        if self.s.task_mode == m : 
            return m
        self.c.state(m)   
        self.c.wait_complete()
        return m

    def do_home(self,axismask):
        self.s.poll()
        self.c.home(axismask)   
        self.c.wait_complete()


    def ok_for_mdi(self):
        ''' 
        check wether ok to run MDI commands.
        '''
        self.s.poll()
        return not self.s.estop and self.s.enabled and self.s.homed 
        
    def prepare_for_mdi(self):
        ''' 
        check wether ok to run MDI commands.
        throw  LinuxcncError if told so.
        return current mode
        '''

        self.s.poll()
        if self.s.estop:
            raise LinuxcncError("machine in ESTOP")
        
        if not self.s.enabled:
            raise LinuxcncError("machine not enabled")
        
        if not self.s.homed:
            raise LinuxcncError("machine not homed")
        
        if self.running():
            raise LinuxcncError("interpreter not idle")
            
        return self.set_mode(linuxcnc.MODE_MDI)

    g_raise_except = True
    
    def g(self,code,wait=False):
        '''
        issue G-Code as MDI command.
        wait for completion if reqested
        '''
        
        self.c.mdi(code)
        if wait:
            try:
                while self.c.wait_complete() == -1:
                    pass
                return True
            except KeyboardInterrupt:
                print "interrupted by keyboard in c.wait_complete()"
                return False

        self.error = self.e.poll()
        if self.error:
            kind, text = self.error
            if kind in (linuxcnc.NML_ERROR, linuxcnc.OPERATOR_ERROR):
                if LinuxcncControl.g_raise_except:
                    raise LinuxcncError(text)
                else:        
                    print ("error " + text)
            else:
                print ("info " + text)
        return False

    def get_current_tool(self):
        self.e.poll()
        return self.e.tool_in_spindle

    def active_codes(self):
        self.e.poll()
        return self.s.gcodes
    
    def get_current_system(self):
        g = self.active_codes()
        for i in g:
                if i >= 540 and i <= 590:
                        return i/10 - 53
                elif i >= 590 and i <= 593:
                        return i - 584
        return 1


def introspect():
    #print "joint.0.select =", h['joint-0-select']
    #print "joint.0.selected =", h['joint-0-selected']
    #print "joint.0.position =", h['joint-0-position']
    os.system("halcmd show pin halui")
    os.system("halcmd show pin python-ui")
    os.system("halcmd show sig")


def do_test():
    # trigger a simple, quick g0 mdi command via halui
    h['trigger-halui-mdi-0'] = 1
    time.sleep(1)
    h['trigger-halui-mdi-0'] = 0
    print "halui mdi g0 went to %.3f" % h['x-position']

    # trigger a probe mdi command via halui
    print "triggering halui mdi command 01 at %s" % time.ctime()
    h['trigger-halui-mdi-1'] = 1
    time.sleep(40)
    print "faking probe contact at %s" % time.ctime()
    h['probe'] = 1
    time.sleep(1)
    h['trigger-halui-mdi-1'] = 0
    print "halui mdi probe went to %.3f" % h['x-position']
    h['probe'] = 0


# reopen stdout file descriptor with write mode
# and 0 as the buffer size (unbuffered)
sys.stdout = os.fdopen(sys.stdout.fileno(), 'w', 0)


#
# set up pins
# shell out to halcmd to make nets to halui and motion
#

h = hal.component("python-ui")
h.newpin("probe", hal.HAL_BIT, hal.HAL_OUT)
h.newpin("x-position", hal.HAL_FLOAT, hal.HAL_IN)
h.newpin("trigger-halui-mdi-0", hal.HAL_BIT, hal.HAL_OUT)
h.newpin("trigger-halui-mdi-1", hal.HAL_BIT, hal.HAL_OUT)
h.ready() # mark the component as 'ready'

os.system("halcmd source ./postgui.hal")


#
# connect to LinuxCNC
#

e = LinuxcncControl()
e.set_state(linuxcnc.STATE_ESTOP_RESET)
e.set_state(linuxcnc.STATE_ON)
e.do_home(0x7)


#
# run the test
#

e.set_mode(linuxcnc.MODE_AUTO)
e.c.program_open('dummy.ngc')

e.set_mode(linuxcnc.MODE_MDI)
e.g("f1");

#introspect()

#
# try probing via mdi from the python gui
#

e.g("o<probe> call");
time.sleep(2)
h['probe'] = 1
e.c.wait_complete()
print "python mdi probe tripped at %.3f" % h['x-position']
h['probe'] = 0


#
# try the test twice, once with the machine in Auto and once in Manual
#

e.set_mode(linuxcnc.MODE_AUTO)
e.c.wait_complete()
do_test()

e.set_mode(linuxcnc.MODE_MANUAL)
e.c.wait_complete()
do_test()


sys.exit(0)

