# Makefile TP Multi-objectif FSP
CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c99 -pedantic -Wno-unused-result -Wno-format-truncation
DEBUG_FLAGS = -Wall -Wextra -g -O0 -std=c99 -DDEBUG -Wno-unused-result -Wno-format-truncation
LDFLAGS = -lm
TARGET = mo
SRC = mflowshop.c
HEADER = mflowshop.h
OBJ = $(SRC:.c=.o)

# Répertoires du projet
INSTANCES_DIR = instances
RESULTS_DIR = results

# Couleurs pour les messages
ifneq ($(TERM),)
	RED = \033[0;31m
	GREEN = \033[0;32m
	YELLOW = \033[0;33m
	BLUE = \033[0;34m
	CYAN = \033[0;36m
	RESET = \033[0m
else
	RED =
	GREEN =
	YELLOW =
	BLUE =
	CYAN =
	RESET =
endif

# ========== Règles principales ==========
.PHONY: all debug clean distclean help run test graphs setup

all: $(TARGET)

debug: CFLAGS = $(DEBUG_FLAGS)
debug: $(TARGET)
	@printf "$(GREEN)✓ Mode debug activé$(RESET)\n"

$(TARGET): $(OBJ)
	@printf "$(BLUE)Liaison...$(RESET)\n"
	$(CC) $(OBJ) -o $@ $(LDFLAGS)
	@printf "$(GREEN)✓ Compilation réussie: $(CYAN)$(TARGET)$(RESET)\n"

%.o: %.c $(HEADER)
	@printf "$(BLUE)Compilation de $(CYAN)$<$(BLUE)...$(RESET)\n"
	$(CC) $(CFLAGS) -c $< -o $@

# ========== Nettoyage ==========
clean:
	@printf "$(YELLOW)Nettoyage...$(RESET)\n"
	rm -f $(OBJ) $(TARGET) test_instance.txt
	@printf "$(GREEN)✓ Fichiers de compilation supprimés$(RESET)\n"

distclean: clean
	@printf "$(YELLOW)Nettoyage complet...$(RESET)\n"
	rm -rf $(RESULTS_DIR)/
	rm -f *.dat *.txt *.png *.gnuplot
	@printf "$(GREEN)✓ Nettoyage complet terminé$(RESET)\n"

# ========== Exécution ==========
run: $(TARGET)
ifndef INSTANCE
	@printf "$(RED)Erreur: Spécifiez une instance avec INSTANCE=fichier.txt$(RESET)\n"
	@printf "Exemples disponibles dans $(CYAN)$(INSTANCES_DIR)/$(RESET):\n"
	@ls $(INSTANCES_DIR)/*.txt 2>/dev/null | xargs -n1 basename | sed 's/^/  - /' || printf "  (aucune instance trouvée)\n"
	@printf "\nExemple: make run INSTANCE=$(INSTANCES_DIR)/7_5_01.txt\n"
	@printf "Exemple: make run INSTANCE=$(INSTANCES_DIR)/20_10_01.txt ARGS=\"-o 1000 -p 20 -v\"\n"
	@exit 1
endif
	@printf "$(GREEN)▶ Exécution sur $(CYAN)$(INSTANCE)$(GREEN)...$(RESET)\n"
	@printf "$(YELLOW)════════════════════════════════════════════════════════════$(RESET)\n"
	@./$(TARGET) $(ARGS) -f ./$(INSTANCE)

# ========== Tests ==========
test: $(TARGET)
	@printf "$(GREEN)▶ Tests rapides...$(RESET)\n"
	@printf "$(YELLOW)1. Test d'aide...$(RESET)\n"
	@./$(TARGET) -h 2>&1 | grep -E "(Usage|Options)" | head -2
	@printf "\n$(YELLOW)2. Test d'arguments...$(RESET)\n"
	@./$(TARGET) 2>&1 | head -1
	@printf "\n$(YELLOW)3. Création instance test...$(RESET)\n"
	@echo "20" > test_instance.txt
	@echo "5" >> test_instance.txt
	@echo "123456789" >> test_instance.txt
	@for i in 0 1 2 3 4; do \
		echo "$$i $$((468 + i*100)) $$((54 + i)) $$((79 + i)) $$((16 + i)) $$((66 + i)) $$((58 + i))" >> test_instance.txt; \
	done
	@printf "$(GREEN)✓ Instance test créée: $(CYAN)test_instance.txt$(RESET)\n"
	@printf "$(YELLOW)4. Test avec instance minimale...$(RESET)\n"
	@./$(TARGET) -o 10 -p 2 -i 10 --no-analyze test_instance.txt 2>&1 | tail -5
	@rm -f test_instance.txt
	@printf "\n$(GREEN)✓ Tests de base OK$(RESET)\n"

# ========== Exécution sur toutes les instances ==========
run-all: $(TARGET)
	@printf "$(GREEN)▶ Exécution sur toutes les instances...$(RESET)\n"
	@for instance in $(INSTANCES_DIR)/*.txt; do \
		if [ -f "$$instance" ]; then \
			printf "\n$(YELLOW)══ Exécution sur $(CYAN)$$instance$(YELLOW) ══$(RESET)\n"; \
			./$(TARGET) $(ARGS) -f "$$instance" 2>&1; \
		fi \
	done
	@printf "\n$(GREEN)✓ Toutes les instances traitées$(RESET)\n"

# ========== Graphiques ==========
graphs:
	@printf "$(GREEN)▶ Génération des graphiques...$(RESET)\n"
	@if command -v gnuplot >/dev/null; then \
		if [ -d "$(RESULTS_DIR)" ]; then \
			printf "$(BLUE)Changement de répertoire vers $(CYAN)$(RESULTS_DIR)$(BLUE)...$(RESET)\n"; \
			cd $(RESULTS_DIR) && \
			count=0; \
			for script in *.gnuplot; do \
				if [ -f "$$script" ]; then \
					printf "$(BLUE)↠ Exécution de $(CYAN)$$script$(BLUE)...$(RESET)\n"; \
					gnuplot "$$script" 2>&1 | grep -v "warning\|line.*:" || true; \
					if [ $$? -eq 0 ]; then \
						count=$$((count + 1)); \
					fi \
				fi \
			done; \
			if [ "$$count" -eq 0 ]; then \
				printf "$(YELLOW)⚠ Aucun script gnuplot exécuté avec succès dans $(CYAN)$(RESULTS_DIR)/$(RESET)\n"; \
				printf "Vérifiez que les fichiers .dat existent dans le même répertoire.\n"; \
			else \
				printf "$(GREEN)✓ Graphiques générés ($(CYAN)$$count$(GREEN) scripts)$(RESET)\n"; \
				printf "Fichiers PNG créés:\n"; \
				ls -la *.png 2>/dev/null | awk '{printf "  - %s\n", $$9}' || printf "$(YELLOW)⚠ Aucun fichier PNG généré$(RESET)\n"; \
			fi; \
		else \
			printf "$(YELLOW)⚠ Répertoire $(CYAN)$(RESULTS_DIR)/$(YELLOW) non trouvé$(RESET)\n"; \
			printf "Exécutez d'abord: make run INSTANCE=...\n"; \
		fi; \
	else \
		printf "$(RED)❌ gnuplot non installé$(RESET)\n"; \
		printf "Installation:\n"; \
		printf "  Ubuntu/Debian: sudo apt install gnuplot\n"; \
		printf "  macOS: brew install gnuplot\n"; \
	fi

# ========== Configuration projet ==========
setup:
	@printf "$(GREEN)▶ Configuration projet...$(RESET)\n"
	@mkdir -p $(RESULTS_DIR)
	@printf "$(GREEN)✓ Répertoires créés: $(CYAN)$(RESULTS_DIR)/$(RESET)\n"
	@printf "$(GREEN)✓ Instances disponibles: $(CYAN)$(INSTANCES_DIR)/$(RESET)\n"
	@ls $(INSTANCES_DIR)/*.txt 2>/dev/null | xargs -n1 basename | sed 's/^/  - /' || printf "  (aucune instance)\n"
	@printf "\n$(GREEN)Commandes utiles:$(RESET)\n"
	@printf "  make test                         # Tests rapides\n"
	@printf "  make run INSTANCE=instances/7_5_01.txt\n"
	@printf "  make run INSTANCE=instances/20_10_01.txt ARGS=\"-o 1000 -p 20 -v\"\n"
	@printf "  make run-all ARGS=\"-o 500 -p 10 -i 200\"\n"
	@printf "  make graphs                       # Générer graphiques\n"

# ========== Aide ==========
help:
	@printf "$(GREEN)╔══════════════════════════════════════════════════════╗$(RESET)\n"
	@printf "$(GREEN)║  Makefile pour le projet TP Multi-objectif FSP       ║$(RESET)\n"
	@printf "$(GREEN)╚══════════════════════════════════════════════════════╝$(RESET)\n"
	@echo ""
	@printf "$(YELLOW)Cibles disponibles:$(RESET)\n"
	@printf "  $(GREEN)all$(RESET)      : Compilation standard\n"
	@printf "  $(GREEN)debug$(RESET)    : Compilation avec symboles de débogage\n"
	@printf "  $(GREEN)clean$(RESET)    : Nettoyer fichiers de compilation\n"
	@printf "  $(GREEN)distclean$(RESET): Nettoyer tout (résultats inclus)\n"
	@printf "  $(GREEN)run$(RESET)      : Exécuter sur une instance (INSTANCE=...)\n"
	@printf "  $(GREEN)run-all$(RESET)  : Exécuter sur toutes les instances\n"
	@printf "  $(GREEN)test$(RESET)     : Tests rapides\n"
	@printf "  $(GREEN)graphs$(RESET)   : Générer graphiques (après exécution)\n"
	@printf "  $(GREEN)setup$(RESET)    : Configurer projet\n"
	@printf "  $(GREEN)help$(RESET)     : Afficher cette aide\n"
	@echo ""
	@printf "$(YELLOW)Instances disponibles:$(RESET)\n"
	@ls $(INSTANCES_DIR)/*.txt 2>/dev/null | xargs -n1 basename | sed 's/^/  - /' || printf "  (aucune instance)\n"
	@echo ""
	@printf "$(YELLOW)Utilisation:$(RESET)\n"
	@printf "  make setup                         # Configuration initiale\n"
	@printf "  make                               # Compilation\n"
	@printf "  make run INSTANCE=instances/7_5_01.txt\n"
	@printf "  make run INSTANCE=instances/20_10_01.txt ARGS=\"-o 1000 -p 20 -v\"\n"
	@printf "  make run-all ARGS=\"-o 500 -p 10 -i 200\"\n"
	@printf "  make graphs                        # Générer graphiques\n"
	@echo ""
	@printf "$(YELLOW)Options du programme:$(RESET)\n"
	@printf "  -h, --help                 Affiche l'aide\n"
	@printf "  -o, --offline N            Solutions pour filtrage offline (defaut: 500)\n"
	@printf "  -p, --poids N              Nombre de poids pour approche scalaire (defaut: 10)\n"
	@printf "  -i, --iterations N         Nombre maximal d'itérations (defaut: 500)\n"
	@printf "  -s, --size N               Taille initiale pour approche Pareto (defaut: 10)\n"
	@printf "  -v, --verbose              Mode verbeux\n"
	@printf "  -od, --output-dir DIR           Répertoire de sortie (defaut: .)\n"
	@printf "  -id, --input-dir DIR           Répertoire des instances (defaut: ./instances)\n"
	@printf "  -f, --instance-file FILE       Fichier instance (defaut: 7_5_01.txt)\n"