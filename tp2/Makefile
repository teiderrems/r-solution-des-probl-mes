# Makefile minimaliste pour TP Multi-objectif FSP

CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c99
LDFLAGS = -lm
TARGET = mo
SRC = mflowshop.c
OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TARGET)
	@echo "Test de l'aide:"
	./$(TARGET) -h || true
	@echo
	@echo "Test avec instance minimale:"
	./$(TARGET) -f 7_5_01.txt -id instances -o 10 -p 2 -i 10 --no-analyze -od test_results
	@echo "Test avec arguments complets:"
	./$(TARGET) -f 7_5_01.txt -id instances -o 100 -p 5 -i 50 -s 5 -r 2 -v -od test_results

graphs:
	@if command -v gnuplot >/dev/null; then \
		for script in ./**/*.gnuplot; do \
			if [ -f "$$script" ]; then \
				gnuplot "$$script"; \
			fi \
		done; \
		echo "Graphiques générés"; \
	else \
		echo "gnuplot non installé"; \
	fi

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean test graphs