# # Compiler  
# CXX = g++  

# # Compiler flags  
# CXXFLAGS = -I./include -Wall -Wextra -std=c++17
# LDFLAGS = -lcurl -lgumbo -pthread

# # Directories  
# SRC_DIR = src  
# INC_DIR = include  
# OBJ_DIR = obj  
# BIN_DIR = bin  

# # Source files  
# SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)  
# OBJ_FILES = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)  

# # Output binary  
# TARGET = $(BIN_DIR)/web_crawler  

# # Create necessary directories  
# .PHONY: all clean  

# all: $(TARGET)  

# $(TARGET): $(OBJ_FILES) | $(BIN_DIR)
# 	$(CXX) $(OBJ_FILES) -o $@ $(LDFLAGS) 

# $(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
# 	$(CXX) $(CXXFLAGS) -c $< -o $@

# $(BIN_DIR) $(OBJ_DIR):
# 	mkdir -p $@  

# clean:  
# 	rm -rf $(OBJ_DIR) $(BIN_DIR)/*

CXX = g++

CXXFLAGS = -I./include -Wall -Wextra -std=c++17
LDFLAGS = -lcurl -lgumbo -pthread

SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
BIN_DIR = bin

SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

TARGET = $(BIN_DIR)/web_crawler

.PHONY: all clean $(BIN_DIR) $(OBJ_DIR)

all: $(TARGET)

$(TARGET): $(OBJ_FILES) | $(BIN_DIR)
	$(CXX) $(OBJ_FILES) -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)/*