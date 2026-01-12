set terminal png size 800,600
set output 'results/results.png'
set title 'Comparaison des algorithmes - Instance 20_10_01.txt'
set xlabel 'Algorithme'
set ylabel 'Coût moyen'
set xtics rotate by -45
set key outside
plot 'results/results_echange_20_10_01.txt_5.txt' using 2:xtic(1) title 'Échange' with linespoints, 'results/results_insertion_20_10_01.txt_5.txt' using 2:xtic(1) title 'Insertion' with linespoints
