CPP := g++
DEBUGFLAGS := -ggdb -O1
RELEASEFLAGS := -O3
CPPFLAGS := -std=c++11 -pedantic -W -Wall -Wextra

ASTYLE := astyle
ASTYLEFLAGS := --options=astyle.rc
ASTYLESUFFIX := .orig

TARGET := mj++

# this looks for .cpp/.hpp-files in the current directory and all subdirectories. (recursive-depth = 1)
SOURCE_FILES := $(wildcard *.cpp) $(wildcard **/*.cpp)
HEADER_FILES := $(wildcard *.hpp) $(wildcard **/*.hpp)

all: $(TARGET)

debug $(TARGET): $(SOURCE_FILES:.cpp=.o)
	$(CPP) $(CPPFLAGS) $(DEBUGFLAGS) $^ -o $(TARGET)

# no .o files in release build
release: $(SOURCE_FILES)
	$(CPP) $(CPPFLAGS) $(RELEASEFLAGS) $^ -o $(TARGET)

clean:
	rm -f $(TARGET) $(SOURCE_FILES:.cpp=.o) *~

%.o : %.cpp
	$(CPP) $(CPPFLAGS) $(DEBUGFLAGS) -c $< -o $(<:.cpp=.o)

style: $(SOURCE_FILES) $(HEADER_FILES)
	$(ASTYLE) $(ASTYLEFLAGS) $^ 
	
styleclean:
	rm -f $(addsuffix $(ASTYLESUFFIX), $(SOURCE_FILES) $(HEADER_FILES))
