CXX = g++
CXXFLAGS = -Wall -std=c++17
TARGET = flac_parser
OBJS = main.o libflac.o

# Rule to build the final executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Rule to build main.o
main.o: main.cpp libflac.hpp
	$(CXX) $(CXXFLAGS) -c main.cpp

# Rule to build libflac.o
libflac.o: libflac.cpp libflac.hpp
	$(CXX) $(CXXFLAGS) -c libflac.cpp

# Clean up build files
clean:
	rm -f $(OBJS) $(TARGET)