set terminal png size 1200,800
set output 'experiment_results.png'
set title 'Expérimentation - Coût moyen vs Max Evals'
set xlabel 'Max Evals'
set ylabel 'Coût moyen'
set key outside
set datafile separator "\t"
set datafile commentschars "#"
set xrange [0:11000]
set yrange [0:*]
plot 'experiment_results.txt' using (stringcolumn(3) eq "Marche aléatoire" && stringcolumn(4) eq "Échange" ? column(2) : 1/0):5 with linespoints lc 1 title "Marche aléatoire Échange", 'experiment_results.txt' using (stringcolumn(3) eq "Marche aléatoire" && stringcolumn(4) eq "Insertion" ? column(2) : 1/0):5 with linespoints lc 2 title "Marche aléatoire Insertion", 'experiment_results.txt' using (stringcolumn(3) eq "Climber First" && stringcolumn(4) eq "Échange" ? column(2) : 1/0):5 with linespoints lc 3 title "Climber First Échange", 'experiment_results.txt' using (stringcolumn(3) eq "Climber First" && stringcolumn(4) eq "Insertion" ? column(2) : 1/0):5 with linespoints lc 4 title "Climber First Insertion", 'experiment_results.txt' using (stringcolumn(3) eq "Climber Best" && stringcolumn(4) eq "Échange" ? column(2) : 1/0):5 with linespoints lc 5 title "Climber Best Échange", 'experiment_results.txt' using (stringcolumn(3) eq "Climber Best" && stringcolumn(4) eq "Insertion" ? column(2) : 1/0):5 with linespoints lc 6 title "Climber Best Insertion", 'experiment_results.txt' using (stringcolumn(3) eq "Mon algorithme" && stringcolumn(4) eq "Échange" ? column(2) : 1/0):5 with linespoints lc 7 title "Mon algorithme Échange", 'experiment_results.txt' using (stringcolumn(3) eq "Mon algorithme" && stringcolumn(4) eq "Insertion" ? column(2) : 1/0):5 with linespoints lc 8 title "Mon algorithme Insertion", 'experiment_results.txt' using (stringcolumn(3) eq "Mon algorithme v2" && stringcolumn(4) eq "Échange" ? column(2) : 1/0):5 with linespoints lc 9 title "Mon algorithme v2 Échange", 'experiment_results.txt' using (stringcolumn(3) eq "Mon algorithme v2" && stringcolumn(4) eq "Insertion" ? column(2) : 1/0):5 with linespoints lc 10 title "Mon algorithme v2 Insertion"
