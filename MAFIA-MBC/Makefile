CFLAGS=-Wall -O3

MBC: bin/MBC

bin/MBC: .obj/MBC.o .obj/PTNode.o .obj/TreeWriter.o .obj/BitDb.o .obj/Itemset.o .obj/Transactionset.o .obj/CartesianProduct.o .obj/CartesianProductDb.o .obj/MBCStatic.o
	g++ $(CFLAGS) $^ -o bin/MBC

.obj/%.o: src/%.cpp
	g++ $(CFLAGS) $< -c -o $@

clean:
	rm -f .obj/*.o bin/MBC
