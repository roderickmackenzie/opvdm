set terminal wxt title 'Light intensity - V_{oc} - www.opvdm.com'
set key top left
set xlabel 'Light intensity (Suns)'
set ylabel 'V_{oc} (Volts)'
set format y '%.1le%T'

plot \
'/home/rod/panos/hpc/8/test/voc scan//30e-3/jv_psun_voc.dat' using ($1*0.001):($2*1.0) with lp lw 2 title '30e-3',\
'/home/rod/panos/hpc/8/test/voc scan//40e-3/jv_psun_voc.dat' using ($1*0.001):($2*1.0) with lp lw 2 title '40e-3',\
'/home/rod/panos/hpc/8/test/voc scan//50e-3/jv_psun_voc.dat' using ($1*0.001):($2*1.0) with lp lw 2 title '50e-3',25e-3*1.6*log(3e6*x),25e-3*1.2*log(4e9*x)
