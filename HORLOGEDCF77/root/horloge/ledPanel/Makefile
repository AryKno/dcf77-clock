FLAGS+=-Wall -Wextra -pedantic
LIBNAME=libLedPanel
INCDIR=./include
LIBSDIR=./lib
LIBS+= -lrgbmatrix -ltinyxml2
FILES+= ledPanel.cpp


all: $(LIBNAME).a

$(LIBNAME).o: $(FILES)
	$(CXX) -I$(INCDIR) $(FLAGS) -c -o $@ $< -L$(LIBSDIR) $(LIBS)

$(LIBNAME).a: $(LIBNAME).o
	$(AR) rcs $(LIBSDIR)/$@ $<

clean:
	rm -f core *.o $(LIBSDIR)/$(LIBNAME).a