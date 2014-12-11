CXX = gcc
CXXFLAGS = -g -O3 -Wall

TARGET = dxl_search

TARGET_MAIN = dxl_search.o
LIB = dxl_hal.o crc.o packet_utils.o communications.o high_level_commands.o

all: $(TARGET)

$(TARGET): $(TARGET_MAIN) $(LIB)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f *.o && rm -f $(TARGET) $(SPEED_TEST_OUT)
