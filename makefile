CC=g++
CFLAGS+=-I/media/myproject/git/c_c++/dtl-cpp
CXXFLAGS+=-I/media/myproject/git/c_c++/dtl-cpp
run : strdiff.o clean
	$(CC) -o run $(CXXFLAGS) strdiff.o
strdiff.o : examples/strdiff.cpp
	$(CC) -c $(CXXFLAGS) examples/strdiff.cpp
clean :
	rm run

