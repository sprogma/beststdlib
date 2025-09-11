
set encoding utf8
set datafile separator ";"

# set terminal sixelgd truecolor size 1200,700 enhanced font "Arial,14"
set terminal pngcairo size 1200,700 enhanced font "Arial,14"
set output "histogram.png"

set title "function 'strlen' results"
set xlabel ""
set ylabel "time (ms)"

# set yrange [0:*]

set style data histogram
set style histogram clustered gap 1
set style fill solid 0.8 border -1
set boxwidth 0.8
        set logscale y

set style line 1 lc rgb "#666666" lt 1 lw 1
set grid ytics linestyle 1

set key inside right top vertical

set xtics rotate by -90
# set xtic rotate by 0 scale 0

plot 'data.csv' using 2:xtic(1) title columnheader(2), \
                        ''         using 3        title columnheader(3), \
                ''         using 4        title columnheader(4),  

set key off
set format y "%g"
ncols = 2
delta = 0.18
        