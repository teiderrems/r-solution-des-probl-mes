#!/usr/bin/env python3
"""Generateur simple de galerie pour les résultats TP1 et TP2.
Usage:
  ./generate_gallery.py --tp1 tp1/results/experiment_results.csv --tp2 tp2/test_results_front/experiment_results_tp2.csv --out gallery
Ce script lit les CSV, genere des graphiques PNG et un fichier HTML `index.html`.
"""
import os
import sys
import argparse
from pathlib import Path

try:
    import pandas as pd
    import matplotlib.pyplot as plt
    import seaborn as sns
except Exception as e:
    print("Dependencies manquantes: installez pandas, matplotlib, seaborn")
    print("pip install pandas matplotlib seaborn")
    sys.exit(1)

sns.set(style='whitegrid')


def ensure_dir(p):
    os.makedirs(p, exist_ok=True)


def plot_tp1(df, outdir):
    # Pour chaque instance, plot CoûtMoyen vs MaxEvals pour chaque Algo
    for inst, g in df.groupby('Instance'):
        plt.figure(figsize=(8,5))
        for (algo, voisin), sub in g.groupby(['Algo','Voisinage']):
            # convertir MaxEvals en int
            sub = sub.copy()
            try:
                sub['MaxEvals'] = sub['MaxEvals'].astype(float)
                sub = sub.sort_values('MaxEvals')
            except Exception:
                pass
            plt.plot(sub['MaxEvals'], sub['CoûtMoyen'], marker='o', label=f"{algo} ({voisin})")
        plt.title(f"TP1 - {inst} : Coût moyen vs MaxEvals")
        plt.xlabel('MaxEvals')
        plt.ylabel('Coût moyen')
        plt.legend(fontsize='small')
        plt.tight_layout()
        fn = os.path.join(outdir, f"tp1_{inst}_cost.png")
        plt.savefig(fn)
        plt.close()


def plot_tp2(df, outdir):
    # Pour chaque instance et ParetoSize, plot Hypervolume vs MaxIter par Algo
    for (inst, pareto), g in df.groupby(['Instance','ParetoSize']):
        plt.figure(figsize=(8,5))
        for algo, sub in g.groupby('Algo'):
            sub = sub.copy()
            try:
                sub['MaxIter'] = sub['MaxIter'].astype(float)
                sub = sub.sort_values('MaxIter')
            except Exception:
                pass
            plt.plot(sub['MaxIter'], sub['Hypervolume'], marker='o', label=algo)
        plt.title(f"TP2 - {inst} (ParetoSize={pareto}) : Hypervolume vs MaxIter")
        plt.xlabel('MaxIter')
        plt.ylabel('Hypervolume')
        plt.legend(fontsize='small')
        plt.tight_layout()
        fn = os.path.join(outdir, f"tp2_{inst}_p{pareto}_hypervol.png")
        plt.savefig(fn)
        plt.close()


def make_html(outdir, tp1_csv, tp2_csv, imgs):
    html = []
    html.append("<!doctype html>")
    html.append("<html lang=\"fr\">")
    html.append("<head><meta charset=\"utf-8\"><title>Galerie Résultats TP1 & TP2</title>")
    html.append("<style>\n  body{font-family:Arial,Helvetica,sans-serif;margin:20px;}\n  img{max-width:100%;height:auto;border:1px solid #ddd;padding:4px;margin:8px 0;}\n\n  /* Grille : 2 colonnes par défaut, 1 colonne sur petits écrans */\n  .grid{\n    display:grid;\n    grid-template-columns: repeat(2, 1fr);\n    gap:16px;\n    align-items:start;\n  }\n  @media (max-width:700px){\n    .grid{grid-template-columns: 1fr;}\n  }\n\n  .card{border:1px solid #eee;padding:10px;border-radius:6px;display:flex;flex-direction:column;align-items:center;}\n  .card img{width:100%;height:360px;object-fit:cover;border-radius:4px;margin:8px 0;border:1px solid #ddd;padding:2px;}\n\n  /* Lightbox overlay */\n  .overlay{position:fixed;top:0;left:0;width:100%;height:100%;background:rgba(0,0,0,0.85);display:none;align-items:center;justify-content:center;z-index:1000;padding:20px;}\n  .overlay img{max-width:90%;max-height:90%;border-radius:6px;box-shadow:0 6px 18px rgba(0,0,0,0.5);}\n  .overlay .close{position:absolute;top:18px;right:24px;color:#fff;font-size:36px;line-height:1;cursor:pointer;padding:6px;background:transparent;border:none;}\n\n  table{border-collapse:collapse;width:100%;}\n  td,th{border:1px solid #ddd;padding:6px;text-align:left;}\n</style>")
    html.append("</head><body>")
    html.append("<h1>Galerie — Résultats TP1 & TP2</h1>")

    if tp1_csv and os.path.exists(tp1_csv):
        html.append("<h2>TP1 — Résultats (CSV)</h2>")
        df1 = pd.read_csv(tp1_csv)
        html.append(df1.head(10).to_html(index=False, classes='card'))

    if tp2_csv and os.path.exists(tp2_csv):
        html.append("<h2>TP2 — Résultats (CSV)</h2>")
        df2 = pd.read_csv(tp2_csv)
        html.append(df2.head(10).to_html(index=False, classes='card'))

    html.append("<h2>Graphiques générés</h2>")

    # Séparer les images en sections
    tp1_imgs = [img for img in imgs if os.path.basename(img).startswith('tp1_') and '_cost' in os.path.basename(img)]
    # For TP2, only include images that contain 'hyper' or 'front' (case-insensitive)
    tp2_hyper = [img for img in imgs if 'hyper' in os.path.basename(img).lower()]
    tp2_pareto = [img for img in imgs if 'front' in os.path.basename(img).lower()]

    # TP1 section
    if tp1_imgs:
        html.append('<h3>TP1 — Plots Coût</h3>')
        html.append('<div class="grid">')
        for img in tp1_imgs:
            name = os.path.basename(img)
            html.append(f"<div class=\"card\"><h4>{name}</h4><a href=\"{name}\" class=\"lightbox-trigger\" data-caption=\"{name}\"><img src=\"{name}\" alt=\"{name}\"></a></div>")
        html.append('</div>' )

    # TP2 hypervolume section
    if tp2_hyper:
        html.append('<h3>TP2 — Hypervolume</h3>')
        html.append('<div class="grid">')
        for img in tp2_hyper:
            name = os.path.basename(img)
            html.append(f"<div class=\"card\"><h4>{name}</h4><a href=\"{name}\" class=\"lightbox-trigger\" data-caption=\"{name}\"><img src=\"{name}\" alt=\"{name}\"></a></div>")
        html.append('</div>')

    # TP2 Pareto/fronts section
    if tp2_pareto:
        html.append('<h3>TP2 — Fronts Pareto / Comparaisons</h3>')
        html.append('<div class="grid">')
        for img in tp2_pareto:
            name = os.path.basename(img)
            html.append(f"<div class=\"card\"><h4>{name}</h4><a href=\"{name}\" class=\"lightbox-trigger\" data-caption=\"{name}\"><img src=\"{name}\" alt=\"{name}\"></a></div>")
        html.append('</div>')

    html.append('<div id="overlay" class="overlay" aria-hidden="true">')
    html.append('  <button class="close" id="lb-close" aria-label="Fermer">&times;</button>')
    html.append('  <img id="overlay-img" src="" alt="">')
    html.append('  <div id="overlay-caption" style="color:#fff;margin-top:8px;text-align:center;"></div>')
    html.append('</div>')
    html.append('<script>')
    html.append('(function(){')
    html.append("  const overlay = document.getElementById('overlay');")
    html.append("  const overlayImg = document.getElementById('overlay-img');")
    html.append("  const caption = document.getElementById('overlay-caption');")
    html.append("  const closeBtn = document.getElementById('lb-close');")
    html.append("  function openLightbox(src, captionText){")
    html.append("    overlayImg.src = src;")
    html.append("    caption.textContent = captionText || ''; ")
    html.append("    overlay.style.display = 'flex';")
    html.append("    overlay.setAttribute('aria-hidden', 'false');")
    html.append("    document.body.style.overflow = 'hidden';")
    html.append("  }")
    html.append("  function closeLightbox(){")
    html.append("    overlay.style.display = 'none';")
    html.append("    overlayImg.src = '';")
    html.append("    overlay.setAttribute('aria-hidden', 'true');")
    html.append("    document.body.style.overflow = '';")
    html.append("  }")
    html.append("  document.querySelectorAll('a.lightbox-trigger').forEach(function(a){")
    html.append("    a.addEventListener('click', function(e){")
    html.append("      e.preventDefault();")
    html.append("      openLightbox(a.href, a.getAttribute('data-caption') || a.title || (a.querySelector('img') && a.querySelector('img').alt) || '');")
    html.append("    });")
    html.append("  });")
    html.append("  overlay.addEventListener('click', function(e){")
    html.append("    if(e.target === overlay || e.target === closeBtn){")
    html.append("      closeLightbox();")
    html.append("    }")
    html.append("  });")
    html.append("  closeBtn.addEventListener('click', closeLightbox);")
    html.append("  document.addEventListener('keydown', function(e){")
    html.append("    if(e.key === 'Escape' && overlay.style.display === 'flex') closeLightbox();")
    html.append("  });")
    html.append("})();")
    html.append('</script>')
    html.append('</body></html>')
    with open(os.path.join(outdir, 'index.html'), 'w', encoding='utf-8') as f:
        f.write('\n'.join(html))
    print('Gallery generated:', os.path.join(outdir, 'index.html'))


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--tp1', default='tp1/results/experiment_results.csv')
    parser.add_argument('--tp2', default='tp2/test_results_front/experiment_results_tp2.csv')
    parser.add_argument('--out', default='gallery')
    parser.add_argument('--generate-pareto', action='store_true', help='Run gnuplot scripts in tp2/test_results_front to generate Pareto/front comparison PNGs')
    args = parser.parse_args()

    outdir = args.out
    ensure_dir(outdir)

    imgs = []

    if os.path.exists(args.tp1):
        df1 = pd.read_csv(args.tp1)
        plot_tp1(df1, outdir)
        imgs += sorted([str(p) for p in Path(outdir).glob('tp1_*_cost.png')])
    else:
        print('TP1 CSV not found:', args.tp1)

    if os.path.exists(args.tp2):
        df2 = pd.read_csv(args.tp2)
        plot_tp2(df2, outdir)
        imgs += sorted([str(p) for p in Path(outdir).glob('tp2_*_hypervol.png')])

        # Directory with Gnuplot-generated images and scripts
        pareto_src = Path('tp2/test_results_front')

        # If requested, run gnuplot scripts to (re)generate front comparison PNGs
        if args.generate_pareto:
            import subprocess, shutil
            if not shutil.which('gnuplot'):
                print('gnuplot not found in PATH, cannot generate pareto images')
            else:
                for script in pareto_src.glob('*front_comparaison.gnuplot'):
                    try:
                        subprocess.run(['gnuplot', str(script)], check=True)
                        print('Produced:', script.stem + '.png')
                    except Exception as e:
                        print('Failed to run gnuplot on', script, ':', e)

        # Collect Gnuplot-generated images: only include files that contain 'front' or 'hyper' in their name
        pareto_pngs = set()
        hyper_pngs = set()

        for p in pareto_src.glob('*.png'):
            name = p.name
            lname = name.lower()
            # Only accept images that explicitly contain 'front' (front_comparaison, _front_) or 'hyper' (hypervolume, hypervol)
            if 'front' in lname:
                pareto_pngs.add(str(p))
            elif 'hyper' in lname:
                hyper_pngs.add(str(p))

        # Copy hypervolume PNGs (Gnuplot) into outdir and prefer them over generated hypervol images
        import shutil
        for p in sorted(hyper_pngs):
            dst = Path(outdir) / Path(p).name
            try:
                shutil.copy2(p, dst)
                imgs.append(str(dst))
            except Exception as e:
                print('Warning: failed to copy', p, '->', dst, ':', e)

        # Remove generated tp2_*_hypervol.png from imgs if we copied any Gnuplot hypervolume PNGs
        if hyper_pngs:
            imgs = [i for i in imgs if not (Path(i).name.startswith('tp2_') and '_hypervol' in Path(i).name)]

        # Copy pareto/front PNGs into outdir so HTML links are local
        for p in sorted(pareto_pngs):
            dst = Path(outdir) / Path(p).name
            try:
                shutil.copy2(p, dst)
                imgs.append(str(dst))
            except Exception as e:
                print('Warning: failed to copy', p, '->', dst, ':', e)
    else:
        print('TP2 CSV not found:', args.tp2)

    # Copy generated images to be referenced relatively in HTML (they are already in outdir)
    make_html(outdir, args.tp1 if os.path.exists(args.tp1) else None, args.tp2 if os.path.exists(args.tp2) else None, imgs)

if __name__ == '__main__':
    main()
