CPP := g++
DEBUGFLAGS := -ggdb -O1
RELEASEFLAGS := -O3
CPPFLAGS := -std=c++14 -pedantic -W -Wall -Wextra

TARGET := mj++
FILES := main.cpp \
	lexer/lexer.cpp \
	lexer/stateomat.cpp \
	lexer/token.cpp

all: $(TARGET)

debug $(TARGET): $(FILES:.cpp=.o)
	$(CPP) $(CPPFLAGS) $(DEBUGFLAGS) $^ -o $(TARGET)

# no .o files in release build
release: $(FILES)
	$(CPP) $(CPPFLAGS) $(RELEASEFLAGS) $^ -o $(TARGET)

clean:
	rm -f $(TARGET) $(FILES:.cpp=.o) *~

%.o : %.cpp
	$(CPP) $(CPPFLAGS) $(DEBUGFLAGS) -c $< -o $(<:.cpp=.o)
