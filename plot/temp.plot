set terminal wxt title 'Current Density - Voltage - www.opvdm.com'
set key top left
set xlabel 'Applied Bias (Volts)'
set ylabel 'Current Density (Am^{-2})'
set format y '%.1le%T'
plot \
'/home/rod/brian/hpc/7/orig/scan1/1000.0/jvexternal.dat' using ($1*1.0):($2*1.0) with lp lw 2 title '1000.0',\
'/home/rod/brian/hpc/7/orig/scan1/47000.0/jvexternal.dat' using ($1*1.0):($2*1.0) with lp lw 2 title '47000.0'