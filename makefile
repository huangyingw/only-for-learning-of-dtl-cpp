run : strdiff.o
	g++ -o run strdiff.o
strdiff.o : examples/strdiff.cpp
	g++ -c examples/strdiff.cpp
clean :
	rm run strdiff.o
