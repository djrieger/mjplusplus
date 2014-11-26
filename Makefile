CPP := g++
DEBUGFLAGS := -ggdb -O1
RELEASEFLAGS := -O3
PROFILEFLAGS := -pg -ggdb -O3
CPPFLAGS := -std=c++1y -pedantic -W -Wall -Wextra -I/usr/local/include
FIRMFLAGS := -lfirm -L/usr/local/lib 

ASTYLE := astyle
ASTYLEFLAGS := --options=config/astyle.rc
ASTYLESUFFIX := .orig

TARGET := mj++

# this looks for .cpp/.hpp-files in the current directory and all subdirectories. (recursive-depth = 1)
SOURCE_FILES := $(sort $(wildcard src/*.cpp)) $(sort $(wildcard src/**/*.cpp)) $(sort $(wildcard src/**/**/*.cpp))
HEADER_FILES := $(sort $(wildcard src/*.hpp)) $(sort $(wildcard src/**/*.hpp)) $(sort $(wildcard src/**/**/*.hpp))

all: $(TARGET)

debug $(TARGET): $(SOURCE_FILES:.cpp=.o)
	$(CPP) $(CPPFLAGS) $(DEBUGFLAGS) $^ -o $(TARGET) $(FIRMFLAGS)

# analyse with gprof [options] ./mj++ gmon.out
profile: $(SOURCE_FILES)
	$(CPP) $(CPPFLAGS) $(PROFILEFLAGS) $^ -o $(TARGET) $(FIRMFLAGS)

# no .o files in release build
release: $(SOURCE_FILES)
	$(CPP) $(CPPFLAGS) $(RELEASEFLAGS) $^ -o $(TARGET) $(FIRMFLAGS)

clean:
	rm -f $(TARGET) $(SOURCE_FILES:.cpp=.o) gmon.out *~

%.o : %.cpp
	$(CPP) $(CPPFLAGS) $(DEBUGFLAGS) -c $< -o $(<:.cpp=.o)

style: $(SOURCE_FILES) $(HEADER_FILES)
	$(ASTYLE) $(ASTYLEFLAGS) $^ 
	
styleclean:
	rm -f $(addsuffix $(ASTYLESUFFIX), $(SOURCE_FILES) $(HEADER_FILES))

doc: $(SOURCE_FILES) $(HEADER_FILES) 
	doxygen config/doxygen.config

.PHONY: doc
