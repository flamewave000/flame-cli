SRC=src/cli.cpp
OBJ=cli.o
PNAME=cli
SAMPLE=samples/main.cpp
CFLAGS=-static -Lstringx/lib -lstringx -Istringx/include -Ilinq/include -Iinclude
SCFLAGS=-Iinclude


library: $(SRC)
	cd stringx && make library 
	g++ -std=c++14 $(CFLAGS) -c $(SRC)
	mkdir -p lib
	ar rcs lib/lib$(PNAME).a $(OBJ)
	rm $(OBJ)
	
sample: library $(SAMPLE)
	g++ -std=c++14 $(SCFLAGS) -static $(SAMPLE) -Llib -l$(PNAME) -o $(PNAME)-sample
	
clean:
	cd stringx && make clean
	rm -Rf lib/ $(PNAME)-sample $(OBJ) || true
