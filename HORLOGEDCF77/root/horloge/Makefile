FLAGS+=-Wall -Wextra -pedantic -g
INCDIR=-I./ledPanel/include -I./Bluetooth/server #-I./ledPanel/RSS
LIBS=-L./ledPanel/lib -lLedPanel -lrgbmatrix  -L./Bluetooth/server -lBluetoothServer  -L./ledPanel/RSS -ltinyxml2 -lpthread -lbluetooth
FILES+= Horloge.cpp
BIN=Horloge


all: $(BIN)

$(BIN).o: $(FILES)
	$(CXX) $(INCDIR) $(FLAGS) -c -o $@ $<

$(BIN): $(BIN).o
	$(CXX) $(FLAGS) $(INCDIR) $(BIN).o $(LIBS) -o $(BIN)

clean:
	rm -f core *.o $(BIN)
