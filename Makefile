CC=gcc
LIBS=-lm -lz -lpthread

all: map convpoly regrid
	rm bin/*.o

map: bin/map.o bin/features.o bin/grid.o bin/model.o bin/matrix.o bin/stats.o bin/image.o
	$(CC) $(LIBS) bin/map.o bin/features.o bin/grid.o bin/model.o bin/matrix.o bin/stats.o bin/image.o -o bin/map

convpoly: bin/convpoly.o bin/polygons.o bin/stats.o bin/matrix.o
	$(CC) $(LIBS) bin/convpoly.o bin/polygons.o bin/stats.o bin/matrix.o -o bin/convpoly

regrid: bin/regrid.o bin/grid.o bin/polygons.o bin/features.o bin/stats.o bin/matrix.o
	$(CC) $(LIBS) bin/regrid.o bin/grid.o bin/polygons.o bin/features.o bin/stats.o bin/matrix.o -o bin/regrid

bin/regrid.o: src/regrid.c
	$(CC) -c src/regrid.c -o bin/regrid.o

bin/convpoly.o: src/convpoly.c
	$(CC) -c src/convpoly.c -o bin/convpoly.o

bin/polygons.o: src/polygons.c
	$(CC) -c src/polygons.c -o bin/polygons.o

bin/map.o: src/map.c
	$(CC) -c src/map.c -o bin/map.o

bin/features.o: src/features.c
	$(CC) -c src/features.c -o bin/features.o

bin/grid.o: src/grid.c
	$(CC) -c src/grid.c -o bin/grid.o

bin/model.o: src/model.c
	$(CC) -c src/model.c -o bin/model.o

bin/image.o: src/image.c
	$(CC) -c src/image.c -o bin/image.o

bin/matrix.o: src/math/matrix.c
	$(CC) -c src/math/matrix.c -o bin/matrix.o

bin/stats.o: src/math/stats.c
	$(CC) -c src/math/stats.c -o bin/stats.o

clean:
	rm bin/*