#!/usr/bin/gnuplot

set logscale x 2;
plot "cm1_plot.dat" using 1:2 with lines title "128kB",  \
     "cm1_plot.dat" using 1:3 with lines title "256kB", \
     "cm1_plot.dat" using 1:4 with lines title "512kB", \
     "cm1_plot.dat" using 1:5 with lines title "1024kB";

pause mouse
