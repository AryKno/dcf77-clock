FLAGS+=-Wall -Wextra -pedantic
LIBNAME=libLedPanel
INCDIR=./include
LIBSDIR=./lib
LIBS+= -lrgbmatrix -ltinyxml2
FILES+= ledPanel.cpp
BIN=heure

all: $(LIBNAME).a $(BIN) 

$(LIBNAME).o: $(FILES)
	$(CXX) -I$(INCDIR) $(FLAGS) -c -o $@ $< -L$(LIBSDIR) $(LIBS)

$(LIBNAME).a: $(LIBNAME).o
	$(AR) rcs $(LIBSDIR)/$@ $<

#make pour le fichier heure
$(BIN): $(BIN).cpp
	$(CXX) $(BIN).cpp -I$(INCDIR) $(FLAGS) -o $(BIN) -L$(LIBSDIR) -lrgbmatrix -lpthread

clean:
	rm -f core *.o $(LIBSDIR)/$(LIBNAME).a
