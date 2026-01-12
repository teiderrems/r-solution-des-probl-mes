set terminal png size 800,600
set output 'results_7_5_01.txt_5.png'
set title 'Comparaison des algorithmes - Instance 7_5_01.txt'
set xlabel 'Algorithme'
set ylabel 'Coût moyen'
set xtics rotate by -45
set key outside
set datafile separator "\t"
plot 'results_echange_7_5_01.txt_5.txt' using 0:2:xtic(1) title 'Échange' with linespoints, 'results_insertion_7_5_01.txt_5.txt' using 0:2:xtic(1) title 'Insertion' with linespoints
