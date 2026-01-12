# Makefile minimaliste pour TP Flow Shop

CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c99
LDFLAGS = -lm
TARGET = fsp
SRC = fsp.c
OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

experiment: $(TARGET)
	@echo "Lancement de l'expérimentation sur toutes les instances..."
	./$(TARGET) --experiment -o results

test: $(TARGET)
	@echo "Test de l'aide:"
	./$(TARGET) -h || true
	@echo "Test d'exécution simple:"
	./$(TARGET) -i instances -f 7_5_01.txt -k 5 -m 10000 -v || true

graphs: $(TARGET)
	@echo "Génération des graphiques..."
	@cd results && for script in *.gnuplot; do echo "Exécution de $$script"; gnuplot $$script; done

.PHONY: all clean test graphs