set terminal png size 800,600
set output 'hypervolume_vs_instance_max_iter_100_pareto_size_5.png'
set title 'Config MaxIter 100, ParetoSize 5 - Hypervolume vs Instance'
set xlabel 'Instance'
set ylabel 'Hypervolume moyen'
set key outside
set datafile separator "\t"
set datafile commentschars "#"
set xtics rotate by -45
set xtics ("7_5_01.txt" 1, "20_10_01.txt" 2, "20_20_01.txt" 3, "30_10_01.txt" 4, "30_20_01.txt" 5, "50_10_01.txt" 6, "50_20_01.txt" 7)
set yrange [0:*]
plot 'experiment_results_tp2.txt' using (column(2) == 100 && column(3) == 5 && stringcolumn(4) eq "scalar" ? (stringcolumn(1) eq "7_5_01.txt" ? 1 : stringcolumn(1) eq "20_10_01.txt" ? 2 : stringcolumn(1) eq "20_20_01.txt" ? 3 : stringcolumn(1) eq "30_10_01.txt" ? 4 : stringcolumn(1) eq "30_20_01.txt" ? 5 : stringcolumn(1) eq "50_10_01.txt" ? 6 : stringcolumn(1) eq "50_20_01.txt" ? 7 : 1/0) : 1/0):5 with linespoints lc 1 title "scalar", 'experiment_results_tp2.txt' using (column(2) == 100 && column(3) == 5 && stringcolumn(4) eq "pareto" ? (stringcolumn(1) eq "7_5_01.txt" ? 1 : stringcolumn(1) eq "20_10_01.txt" ? 2 : stringcolumn(1) eq "20_20_01.txt" ? 3 : stringcolumn(1) eq "30_10_01.txt" ? 4 : stringcolumn(1) eq "30_20_01.txt" ? 5 : stringcolumn(1) eq "50_10_01.txt" ? 6 : stringcolumn(1) eq "50_20_01.txt" ? 7 : 1/0) : 1/0):5 with linespoints lc 2 title "pareto"
