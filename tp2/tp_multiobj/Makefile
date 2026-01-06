# Compilateur
CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c99
TARGET = tp_multiobj
INSTANCE_DIR = ../../instances
SOURCES = main.c config.c instance.c evaluation.c archive.c algorithms.c analysis.c utils.c
OBJECTS = $(SOURCES:.c=.o)

# Règle par défaut
all: $(TARGET)

# Création de l'exécutable
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) -lm

# Règle générique pour compiler les fichiers .c en .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Dépendances des fichiers .h
config.o: config.h utils.h
instance.o: instance.h
evaluation.o: evaluation.h instance.h
archive.o: archive.h evaluation.h
algorithms.o: algorithms.h instance.h archive.h config.h evaluation.h analysis.h utils.h
analysis.o: analysis.h archive.h config.h algorithms.h utils.h
utils.o: utils.h
main.o: config.h instance.h evaluation.h archive.h algorithms.h analysis.h utils.h

# Nettoyage
clean:
	rm -f $(OBJECTS) $(TARGET)

# Nettoyage complet
cleanall: clean
	rm -f *.dat *.txt

# Exécution avec exemple
run: $(TARGET)
	./$(TARGET) -o 500 -p 10 -i 1000 ./$(INSTANCE_DIR)/7_5_01.txt

# Débogage
debug: CFLAGS += -g -DDEBUG
debug: clean $(TARGET)

# Profilage
profile: CFLAGS += -pg
profile: clean $(TARGET)

# Installation
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

.PHONY: all clean cleanall run debug profile install