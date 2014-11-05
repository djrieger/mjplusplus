CPP := g++
DEBUGFLAGS := -ggdb -O1
RELEASEFLAGS := -O3
CPPFLAGS := -std=c++11 -pedantic -W -Wall -Wextra

ASTYLE := astyle
ASTYLEFLAGS := --options=astyle.rc
ASTYLESUFFIX := .orig

TARGET := mj++
SOURCE_FILES := main.cpp \
	lexer/lexer.cpp \
	lexer/stateomat.cpp \
	lexer/token.cpp \
	parser/parser.cpp
	
HEADER_FILES := lexer/lexer.hpp \
	lexer/stateomat.hpp \
	lexer/token.hpp \
	parser/parser.hpp

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
