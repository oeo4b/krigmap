CC=gcc
LIBS=-lm

map: bin/map.o bin/features.o bin/grid.o bin/model.o bin/matrix.o bin/stats.o
	$(CC) $(LIBS) bin/map.o bin/features.o bin/grid.o bin/model.o bin/matrix.o bin/stats.o -o bin/map
	rm bin/*.o

bin/map.o: src/map.c
	$(CC) -c src/map.c -o bin/map.o

bin/features.o: src/features.c
	$(CC) -c src/features.c -o bin/features.o

bin/grid.o: src/grid.c
	$(CC) -c src/grid.c -o bin/grid.o

bin/model.o: src/model.c
	$(CC) -c src/model.c -o bin/model.o

bin/matrix.o: src/math/matrix.c
	$(CC) -c src/math/matrix.c -o bin/matrix.o

bin/stats.o: src/math/stats.c
	$(CC) -c src/math/stats.c -o bin/stats.o

clean:
	rm bin/map