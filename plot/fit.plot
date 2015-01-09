#set term postscript eps enhanced color "Helvetica" 8
set ylabel 'Delta T/T_f - delta T/T'
set xlabel 'Time (s)'
unset logscale
plot \
'./exp/celiv0/data.dat' using ($1):($2) with lp lw 4 title 'exp',\
'./sim/celiv0/celiv_i.dat' using ($1):($2) with lp lw 4 title 'sim',\
'./sim/celiv0/celiv_i.dat.back' using ($1):($2) with lp lw 2 title 'sim last'
