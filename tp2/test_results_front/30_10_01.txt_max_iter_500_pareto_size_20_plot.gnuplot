set terminal png size 800,600
set output '30_10_01.txt_max_iter_500_pareto_size_20.png'
set title 'Instance 30_10_01.txt, MaxIter 500, ParetoSize 20 - Hypervolume'
set xlabel 'Algorithm'
set ylabel 'Hypervolume'
set xtics ('scalar' 1, 'pareto' 2)
plot 'experiment_results_tp2.txt' using (stringcolumn(1) eq "30_10_01.txt" && $2 == 500 && $3 == 20 && stringcolumn(4) eq "scalar" ? 1 : 1/0):5 with points pt 7 lc 1 title "scalar", 'experiment_results_tp2.txt' using (stringcolumn(1) eq "30_10_01.txt" && $2 == 500 && $3 == 20 && stringcolumn(4) eq "pareto" ? 2 : 1/0):5 with points pt 7 lc 2 title "pareto"
