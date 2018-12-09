FLAGS+=-Wall -Wextra -pedantic
LIBNAME=libBluetoothServer
LIBS+= -lpthread -lbluetooth
FILES+= rfcomm.c


all: $(LIBNAME).a

$(LIBNAME).o: $(FILES)
	$(CXX) $(FLAGS) -c -o $@ $< -L$(LIBSDIR) $(LIBS)

$(LIBNAME).a: $(LIBNAME).o
	$(AR) rcs $@ $<

clean:
	rm -f core *.o $(LIBNAME).a 