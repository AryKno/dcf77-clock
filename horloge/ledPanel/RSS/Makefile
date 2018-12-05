#CXX est une variable prédéfinie qui correspond à g++
SOURCES+= test.cpp
BIN=test
LIBS+=-ltinyxml2
LIBSDIR=./
CFLAGS+=-g -Wall -Wextra


all: $(SOURCES)
	$(CXX) $(SOURCES) -L$(LIBSDIR) $(LIBS) -o $(BIN)
	#g++ -g test.cpp -L./ -ltinyxml2 -o test.exe -Wall -Wextra 
clean:
	rm -rf $(BIN)

