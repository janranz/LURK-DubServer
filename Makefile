SRC_DIR := source
HEAD_DIR := headers
OBJ_DIR := obj
# DEP_DIR := dep
BIN_DIR := bin
# RM = rm -rv
EXE := $(BIN_DIR)/dubServ
SRC := $(wildcard $(SRC_DIR)/*.cpp)
HEADR := $(wildcard $(HEAD_DIR)/*.h)
OBJS := $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)


CXX=g++
RM=rm -f
CPPFLAGS := -Wall -std=c++11 -Iinclude -MMD -MP
# CFLAGS := -Wall
LDFLAGS := -Llib -lpthread
LDLIBS :=-lm


.PHONY: all clean
all: $(EXE)

$(EXE):$(OBJS) | $(BIN_DIR)
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

clean:
	@$(RM) -rv $(BIN_DIR) $(OBJ_DIR)

-include $(OBJS:.o=.d)
# .PHONY: all
# all: ; $(info $$OBJS is [${SRC}])echo Hello


