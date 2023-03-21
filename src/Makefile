CC = g++
CFLAGS = -std=c++17
CPP = main.cpp server.cpp util/files.cpp util/http.cpp lib/libyaml-cpp.a
BUILDFOLDER = ../bin
TARGET = $(BUILDFOLDER)/mginx

all: $(TARGET)
	$(CC) -o $(TARGET) $(CPP) $(CFLAGS)

$(TARGET): $(BUILDFOLDER)
	touch $@

$(BUILDFOLDER):
	mkdir -p $@