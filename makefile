CC = g++
CC_ARG = -std=c++2a

make: playground.cpp
	${CC} ${CC_ARG} playground.cpp -o a.out

run:
	./a.out

makerun:
	make && make run

clean:
	rm *.out