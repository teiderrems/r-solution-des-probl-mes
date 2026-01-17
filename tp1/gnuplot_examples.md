# Exemples Gnuplot pour TP1

Ce fichier montre comment utiliser les scripts Gnuplot générés par `fsp`/`make graphs` et comment tracer rapidement les fichiers de résultats.

## Scripts générés automatiquement
Le programme `fsp` produit dans `results/` des fichiers de données et des scripts Gnuplot nommés comme :
- `<instance>_plot.gnuplot` (ex. `20_10_01.txt_plot.gnuplot`)
- `experiment_plot.gnuplot` (comparaison globale)

## Exécution d'un script Gnuplot
```bash
# Dans le répertoire racine du projet ou dans results/
cd tp1/results
gnuplot 20_10_01.txt_plot.gnuplot
# ou en chargeant explicitement :
gnuplot -e "load '20_10_01.txt_plot.gnuplot'"
```

## Exemple de script simple (points coût vs max_evals)
```
set terminal png size 800,600
set output '20_10_01_cost_vs_maxevals.png'
set title 'Coût moyen vs Max Evals - Instance 20_10_01'
set xlabel 'Max Evals'
set ylabel 'Coût moyen'
set key outside
set datafile separator "\t"
plot 'experiment_results.txt' using 2:5 index 0 with linespoints lt 1 pt 7 title 'Algorithme A'
```

> Astuce : les scripts générés contiennent déjà la plupart des réglages; adaptez l'`output` et les colonnes si nécessaire.

## Générer tous les PNG d'un coup
```bash
# Lancer depuis tp1/results
for f in *.gnuplot; do gnuplot "$f"; done
```

## Ajout manuel d'un script d'exemple
Vous pouvez créer `tp1/results/my_plot.gnuplot` avec le contenu ci‑dessus et lancer `gnuplot my_plot.gnuplot` pour produire l'image.
