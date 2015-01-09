set multiplot
set size 0.5,0.5
set origin 0.5,0.0

#plot \
#'./exp/jv0/jv.dat',\
#'./sim/jv0/jvexternal.dat'

plot \
'./error_jv_exp0.dat' using ($1):($2) with lp lw 4 lt 2 title 'sim1',\
'./error_jv_sim0.dat' using ($1):($2) with lp lw 4 lt 3 title 'exp1'

set size 0.5,0.5
set origin 0.0,0.0
plot \
'./error_jv_exp1.dat' using ($1):($2) with lp lw 4 lt 2 title 'sim1',\
'./error_jv_sim1.dat' using ($1):($2) with lp lw 4 lt 3 title 'exp1'

set origin 0.0,0.5
plot \
'./error_pulse_voc_sim0.dat' using ($1):($2) with lp lw 4 lt 2 title 'sim1',\
'./error_pulse_voc_exp0.dat' using ($1):($2) with lp lw 4 lt 3 title 'exp1'

