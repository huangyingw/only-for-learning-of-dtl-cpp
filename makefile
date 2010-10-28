run : strdiff.o
	g++ -o run strdiff.o
strdiff.o : examples/strdiff.cpp dtl/Diff.h dtl/Diff.cc
	g++ -c examples/strdiff.cpp dtl/Diff.h dtl/Diff.cc
clean :
	rm run strdiff.o
