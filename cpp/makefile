SRC=src/cli.cpp
OBJ=cli.o
PNAME=cli
SAMPLE=samples/main.cpp
CFLAGS=-static -Lstringx/lib -lstringx -Iinclude -Istringx/include -Ilinq/include
SCFLAGS=-Iinclude -Istringx/include -Ilinq/include


library: $(SRC)
	cd stringx && make library 
	g++ -std=c++14 $(CFLAGS) -c $(SRC)
	mkdir -p lib
	ar rcs lib/lib$(PNAME).a $(OBJ)
	rm $(OBJ)
	
sample: library $(SAMPLE)
	g++ -std=c++14 $(SCFLAGS) -static $(SAMPLE) -Llib -l$(PNAME) -Lstringx/lib -lstringx -o $(PNAME)-sample.o
	
clean:
	cd stringx && make clean
	rm -Rf lib/ $(PNAME)-sample.o $(OBJ) || true
