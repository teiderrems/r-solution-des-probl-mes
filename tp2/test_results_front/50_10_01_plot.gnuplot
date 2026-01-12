set terminal png size 800,600
set output '50_10_01_results.png'
set title 'Instance 50_10_01 - Hypervolume vs Max Iterations'
set xlabel 'Max Iterations'
set ylabel 'Hypervolume moyen'
set key outside
set datafile separator "\t"
set datafile commentschars "#"
set xrange [0:1200]
set yrange [0:*]
plot 'experiment_results_tp2.txt' using (stringcolumn(1) eq "50_10_01" && stringcolumn(4) eq "scalar" && column(3) == 5 ? column(2) : 1/0):5 with linespoints lc 1 title "scalar ParetoSize 5", 'experiment_results_tp2.txt' using (stringcolumn(1) eq "50_10_01" && stringcolumn(4) eq "pareto" && column(3) == 5 ? column(2) : 1/0):5 with linespoints lc 2 title "pareto ParetoSize 5", 'experiment_results_tp2.txt' using (stringcolumn(1) eq "50_10_01" && stringcolumn(4) eq "scalar" && column(3) == 10 ? column(2) : 1/0):5 with linespoints lc 3 title "scalar ParetoSize 10", 'experiment_results_tp2.txt' using (stringcolumn(1) eq "50_10_01" && stringcolumn(4) eq "pareto" && column(3) == 10 ? column(2) : 1/0):5 with linespoints lc 4 title "pareto ParetoSize 10", 'experiment_results_tp2.txt' using (stringcolumn(1) eq "50_10_01" && stringcolumn(4) eq "scalar" && column(3) == 20 ? column(2) : 1/0):5 with linespoints lc 5 title "scalar ParetoSize 20", 'experiment_results_tp2.txt' using (stringcolumn(1) eq "50_10_01" && stringcolumn(4) eq "pareto" && column(3) == 20 ? column(2) : 1/0):5 with linespoints lc 6 title "pareto ParetoSize 20"
