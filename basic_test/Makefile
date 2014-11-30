CXX = gcc
CXXFLAGS = -g -O3 -Wall

TARGET = read_modelnum

TARGET_MAIN = readmodelnum.o
LIB = dxl_hal.o

all: $(TARGET)

$(TARGET): $(TARGET_MAIN) $(LIB)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f *.o && rm -f $(TARGET) $(SPEED_TEST_OUT)
