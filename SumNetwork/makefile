CFLAGS=-Wall -O3

GraphMining: bin/GraphMining

bin/GraphMining: .obj/GraphMatrix.o .obj/NetPattern.o .obj/main.o
	g++ $(CFLAGS) $^ -o bin/GraphMining

.obj/%.o: src/%.cpp
	g++ $(CFLAGS) $< -c -o $@
	
clean:
	rm -f .obj/*.o bin/GraphMining
