set terminal wxt title 'opvdm - JV Curve'
set ylabel 'Current (A)'
set xlabel 'Applied Voltage (V)'
plot 'ivexternal.dat' with lp title 'JV curve',\
'ivexternal_last.dat' with lp title 'JV curve'
