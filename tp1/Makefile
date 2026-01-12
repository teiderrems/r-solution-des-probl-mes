# ==============================================
# Fichier: Makefile
# Projet: Flow Shop (TP)
# ==============================================

# ============ CONFIGURATION ============
CC      = gcc
CFLAGS  = -Wall -Wextra -O2 -g -fdiagnostics-color=always
LDFLAGS = -lm
TARGET  = fsp
SRC     = fsp.c
OBJ     = $(SRC:.c=.o)
HEADER  = header.h

# ============ CIBLES PRINCIPALES ============
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

# ============ UTILITAIRES ============
clean:
	rm -f $(OBJ) $(TARGET) *.out *.exe

distclean: clean
	rm -rf test_results/ profile_data/ 

run: $(TARGET)
	@echo "Exemple d'exécution :"
	@echo "./fsp -k 5 -m 10000 -f instances/20_10_01.txt"

test: $(TARGET)
	@mkdir -p test_results
	@echo "Tests de base en cours.."
	@echo "=== Test 1: Instance 20_10_01 ==="
	@./fsp -v -f instances/20_10_01.txt -k 2 -m 5000 2>&1 | tee test_results/test1.txt
	@echo "=== Test 2: Instance 20_5_01 ==="
	@./fsp -v -f instances/20_5_01.txt -k 2 -m 5000 2>&1 | tee test_results/test2.txt
	@echo "=== Tests terminés ==="

# ============ PROFILAGE ============
profile: CFLAGS += -pg
profile: clean $(TARGET)
	@echo "Compilation avec profilage (gprof)..."
	@./fsp -v -f instances/20_10_01.txt -k 1 -m 10000 -s 12345
	@gprof $(TARGET) gmon.out > profile_data.txt
	@echo "Profilage terminé. Voir profile_data.txt"

# ============ OPTIMISATIONS ============
release: CFLAGS = -Wall -O3 -march=native -flto -DNDEBUG
release: clean $(TARGET)
	@echo "Compilation optimisée (release)"

debug: CFLAGS = -Wall -Wextra -O0 -g -DDEBUG -fsanitize=address,undefined
debug: clean $(TARGET)
	@echo "Compilation debug avec sanitizers"

# ============ DOCUMENTATION ============
docs:
	@echo "Génération de la documentation..."
	@doxygen Doxyfile 2>/dev/null || echo "Installez Doxygen: sudo apt install doxygen graphviz"
	@echo "Documentation générée dans docs/"

# ============ PACKAGING ============
dist: clean
	@mkdir -p dist/$(TARGET)_v1.0
	@cp -r $(SRC) $(HEADER) Makefile README.md instances/ dist/$(TARGET)_v1.0/
	@tar -czf $(TARGET)_v1.0.tar.gz -C dist $(TARGET)_v1.0
	@rm -rf dist
	@echo "Archive créée: $(TARGET)_v1.0.tar.gz"

# ============ AIDES ============
help:
	@echo "========================================"
	@echo "Makefile pour le projet Flow Shop (FSP)"
	@echo "========================================"
	@echo "Cibles disponibles :"
	@echo "  all          : Compile le programme (défaut)"
	@echo "  clean        : Nettoie les fichiers compilés"
	@echo "  run          : Affiche un exemple d'exécution"
	@echo "  test         : Lance des tests de base"
	@echo "  profile      : Compile avec profilage gprof"
	@echo "  release      : Compilation optimisée pour production"
	@echo "  debug        : Compilation avec débogage et sanitizers"
	@echo "  docs         : Génère la documentation Doxygen"
	@echo "  dist         : Crée une archive distribuable"
	@echo "  help         : Affiche ce message"
	@echo ""
	@echo "Exemples :"
	@echo "  make && ./fsp -f instances/20_10_01.txt -k 10 -m 100000"
	@echo "  make && ./fsp -v -f instances/20_10_01.txt -k 10 -m 100000  # Mode verbeux"
	@echo "  make && ./fsp -f instances/20_10_01.txt -k 5 -m 50000 -o results  # Sauvegarde"
	@echo "  make && ./fsp -f instances/20_10_01.txt -k 5 -m 10000 -s 42  # Graine fixe"
	@echo "  make debug && ./fsp -f instances/20_10_01.txt -k 1 -m 1000"
	@echo "  make test   # Lance les tests de base"

# ============ DEPENDANCES ============
$(OBJ): $(HEADER)

.PHONY: all clean run test profile release debug docs dist help