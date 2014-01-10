# HAL config file to check vel/acc constraints
#
loadrt wcomp names=wcomp_xacc,wcomp_xvel,wcomp_yacc,wcomp_yvel,wcomp_zacc,wcomp_zvel,wcomp_wvel,wcomp_wacc
loadrt minmax names=mm_xacc,mm_xvel,mm_yacc,mm_yvel,mm_zacc,mm_zvel,mm_wvel,mm_wacc 
addf wcomp_xacc servo-thread
addf wcomp_xvel servo-thread
addf wcomp_yacc servo-thread
addf wcomp_yvel servo-thread
addf wcomp_zacc servo-thread
addf wcomp_zvel servo-thread
addf wcomp_wacc servo-thread
addf wcomp_wvel servo-thread

addf mm_xacc servo-thread
addf mm_xvel servo-thread
addf mm_yacc servo-thread
addf mm_yvel servo-thread
addf mm_zacc servo-thread
addf mm_zvel servo-thread
addf mm_wacc servo-thread
addf mm_wvel servo-thread

net Xacc => wcomp_xacc.in mm_xacc.in
net Xvel => wcomp_xvel.in mm_xvel.in
net Yacc => wcomp_yacc.in mm_yacc.in
net Yvel => wcomp_yvel.in mm_yvel.in
net Zacc => wcomp_zacc.in mm_zacc.in
net Zvel => wcomp_zvel.in mm_zvel.in
net Wacc => wcomp_wacc.in mm_wacc.in
net Wvel => wcomp_wvel.in mm_wvel.in

net acc-ok-x <= wcomp_xacc.out
net vel-ok-x <= wcomp_xvel.out
net acc-ok-y <= wcomp_yacc.out
net vel-ok-y <= wcomp_yvel.out
net acc-ok-z <= wcomp_zacc.out
net vel-ok-z <= wcomp_zvel.out
net acc-ok-w <= wcomp_wacc.out
net vel-ok-w <= wcomp_wvel.out

#FIXME hard coded acceleration bounds
set constraint_bound 1.0001
setp wcomp_xacc.max $::AXIS_0(MAX_ACCELERATION)*$constraint_bound
setp wcomp_xacc.min $::AXIS_0(MAX_ACCELERATION)*-1.0*$constraint_bound
setp wcomp_xvel.max $::AXIS_0(MAX_VELOCITY)*$constraint_bound
setp wcomp_xvel.min $::AXIS_0(MAX_VELOCITY)*-1.0*$constraint_bound

setp wcomp_yacc.max $::AXIS_1(MAX_ACCELERATION)*$constraint_bound
setp wcomp_yacc.min $::AXIS_1(MAX_ACCELERATION)*-1.0*$constraint_bound
setp wcomp_yvel.max $::AXIS_1(MAX_VELOCITY)*$constraint_bound
setp wcomp_yvel.min $::AXIS_1(MAX_VELOCITY)*-1.0*$constraint_bound

setp wcomp_zacc.max $::AXIS_2(MAX_ACCELERATION)*$constraint_bound
setp wcomp_zacc.min $::AXIS_2(MAX_ACCELERATION)*-1.0*$constraint_bound
setp wcomp_zvel.max $::AXIS_2(MAX_VELOCITY)*$constraint_bound
setp wcomp_zvel.min $::AXIS_2(MAX_VELOCITY)*-1.0*$constraint_bound

setp wcomp_wacc.max $::AXIS_8(MAX_ACCELERATION)*$constraint_bound
setp wcomp_wacc.min $::AXIS_8(MAX_ACCELERATION)*-1.0*$constraint_bound
setp wcomp_wvel.max $::AXIS_8(MAX_VELOCITY)*$constraint_bound
setp wcomp_wvel.min $::AXIS_8(MAX_VELOCITY)*-1.0*$constraint_bound

loadrt match8 names=match_all

addf match_all servo-thread

net acc-ok-x => match_all.a0
setp match_all.b0 1
net vel-ok-x => match_all.a1
setp match_all.b1 1
net acc-ok-y => match_all.a2
setp match_all.b2 1
net vel-ok-y => match_all.a3
setp match_all.b3 1
net acc-ok-z => match_all.a4
setp match_all.b4 1
net vel-ok-z => match_all.a5
setp match_all.b5 1
net acc-ok-w => match_all.a6
setp match_all.b6 1
net vel-ok-w => match_all.a7
setp match_all.b7 1

setp match_all.in 1

net constraints-ok <= match_all.out => motion.enable

