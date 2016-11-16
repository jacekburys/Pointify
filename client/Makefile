SRC := src
BIN := bin

SOURCES := $(shell find $(SRC) -name "*.cpp")
OBJECTS := $(addprefix $(BIN)/,$(addsuffix .o,$(basename $(foreach file,$(SOURCES),$(shell basename $(file))))))

all: $(BIN)/client

$(BIN)/client: $(OBJECTS)
	g++ -Llib/mac \
	    -lc++ -lSystem -lpthread \
	    -lsocketio -lboost_system -lboost_thread \
	    -lfreenect2 \
	    -lopencv_aruco -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs \
	    -o $@ \
	    $^

$(BIN)/camera.o: $(SRC)/camera.cpp $(BIN)
	g++ -c -g -Iinclude -std=c++14 -o $@ $<

$(BIN)/client.o: $(SRC)/client.cpp $(BIN)
	g++ -c -g -Iinclude -std=c++14 -o $@ $<

$(BIN)/calibration.o: $(SRC)/calibration.cpp $(BIN)
	g++ -c -g -Iinclude -std=c++14 -o $@ $<

# NOTE: Change to this if you understand why it's not working
# $(BIN)/%.o: $(SRC)/%.cpp $(BIN)
#  	g++ -c -g -Iinclude -std=c++14 -o $@ $<


$(BIN):
	@mkdir $(BIN)

clean:
	@rm -rf $(BIN)

