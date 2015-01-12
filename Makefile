CPP := g++
DEBUGFLAGS := -ggdb -O1
RELEASEFLAGS := -O3
PROFILEFLAGS := -pg -ggdb -O3
COMMON := -std=c++1y -pedantic -W -Wall -Wextra
CPPFLAGS :=  -Ilibfirm/include -Ilibfirm/build/gen/include/libfirm
LDFLAGS :=  -Llibfirm/build/debug -lfirm

ASTYLE := astyle
ASTYLEFLAGS := --options=config/astyle.rc
ASTYLESUFFIX := .orig

TARGET := mj++

# this looks for .cpp/.hpp-files in the current directory and all subdirectories. (recursive-depth = 1)
SOURCE_FILES := $(sort $(wildcard src/*.cpp)) $(sort $(wildcard src/**/*.cpp)) $(sort $(wildcard src/**/**/*.cpp))
HEADER_FILES := $(sort $(wildcard src/*.hpp)) $(sort $(wildcard src/**/*.hpp)) $(sort $(wildcard src/**/**/*.hpp))

all: $(TARGET)

debug $(TARGET): $(SOURCE_FILES:.cpp=.o) | libfirm/build/debug/libfirm.so
	$(CPP) $(COMMON) $(DEBUGFLAGS) $^ $(LDFLAGS) -o $(TARGET)

# analyse with gprof [options] ./mj++ gmon.out
profile: $(SOURCE_FILES) | libfirm/build/debug/libfirm.so
	$(CPP) $(COMMON) $(CPPFLAGS) $(PROFILEFLAGS) $^ $(LDFLAGS) -o $(TARGET)

# no .o files in release build
release: $(SOURCE_FILES) | libfirm/build/debug/libfirm.so
	$(CPP) $(COMMON) $(CPPFLAGS) $(RELEASEFLAGS) $^ $(LDFLAGS) -o $(TARGET)

libfirm/build/debug/libfirm.so:
	git submodule update --init
	( cd libfirm && make )

clean:
	rm -f $(TARGET) $(SOURCE_FILES:.cpp=.o) gmon.out *~

%.o : %.cpp | libfirm/build/debug/libfirm.so
	$(CPP) $(COMMON) $(CPPFLAGS) $(DEBUGFLAGS) -c $< -o $(<:.cpp=.o)

style: $(SOURCE_FILES) $(HEADER_FILES)
	$(ASTYLE) $(ASTYLEFLAGS) $^ 
	
styleclean:
	rm -f $(addsuffix $(ASTYLESUFFIX), $(SOURCE_FILES) $(HEADER_FILES))

doc: $(SOURCE_FILES) $(HEADER_FILES) 
	doxygen config/doxygen.config
