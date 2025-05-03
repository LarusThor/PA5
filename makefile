CXX = g++
CXXFLAGS = -std=c++17 -Iexternal -pthread

SRC = src/main.cpp
OUT = chat_server

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT)
