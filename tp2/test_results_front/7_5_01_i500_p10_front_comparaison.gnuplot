# ============================================
# Script gnuplot généré automatiquement
# TP Multi-objectif FSP - Analyse graphique
# Instance: instances/7_5_01
# Iterations: 500, Pareto size: 10
# Préfixe: 7_5_01_i500_p10
# ============================================

# Configuration générale
set terminal pngcairo enhanced size 1200,800
set encoding utf8
set datafile separator whitespace
set datafile commentschars "#"
set key top right
set grid
set style line 1 lc rgb '#0060ad' lt 1 lw 2 pt 7 ps 1.5   # Scalaire
set style line 2 lc rgb '#dd181f' lt 1 lw 2 pt 9 ps 1.5   # Pareto
set style line 3 lc rgb '#00ad60' lt 2 lw 2 pt 5 ps 1.0   # Offline

# Titre et labels
set title "Comparaison des fronts Pareto\nInstance: 7_5_01 (Iterations: 500, Pareto size: 10)"
set xlabel 'Makespan (Cmax)'
set ylabel 'Tardiness (Tsum)'
set logscale y

# Fichier de sortie
set output '7_5_01_i500_p10_front_comparaison.png'

# Tracé des données
plot '7_5_01_i500_p10_front_offline.dat' using 1:2 with points ls 3 title 'Filtrage Offline (référence)', '7_5_01_i500_p10_front_scalaire_run_1.dat' using 1:2 with points ls 1 title 'Scalaire Run 1', '7_5_01_i500_p10_front_pareto_run_1.dat' using 1:2 with points ls 2 title 'Pareto Run 1'

# Fin du script
print 'Script exécuté avec succès. Graphique généré: 7_5_01_i500_p10_front_comparaison.png'
