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

test: $(TARGET)
	@echo "Test de l'aide:"
	./$(TARGET) -h || true
	@echo "Test d'exécution simple:"
	./$(TARGET) -i instances -f 7_5_01.txt -k 5 -m 10000 -v || true

graphs: $(TARGET)
	@echo "Génération des graphiques..."
	./$(TARGET) -i instances -f 7_5_01.txt -k 5 -m 10000 -o results
	@cd results && gnuplot plot_7_5_01.txt_5.gnuplot

.PHONY: all clean test graphs