PROGRAM_NAME := kcipher2

ifeq ($(OS),Windows_NT)
	CXX := D:\system\TDM-GCC-64\bin\g++
	MKDIR = if NOT EXIST $(subst /,\,$(1)) md $(subst /,\,$(1))
	BOOST_PATH := D:\system\boost\include\boost-1_59
else
	CXX := g++
	MKDIR = mkdir -p $(1)
	BOOST_PATH := $(HOME)/local/boost/include
endif

CXXFLAGS := -Wall -Wextra -lm -O3 -march=native -fomit-frame-pointer -fopenmp -std=c++11 -Wno-missing-field-initializers -I$(BOOST_PATH)
LIBS :=

OUT_DIR := out
SOURCE_DIR := $(PROGRAM_NAME)
HEADER_DIR := $(PROGRAM_NAME)

SOURCES := $(wildcard $(SOURCE_DIR)/*.cpp)
HEADERS := $(wildcard $(HEADER_DIR)/*.h)

PROGRAM_DIR := $(OUT_DIR)/bin
OBJ_DIR := $(OUT_DIR)/obj
DEPEND_DIR := $(OUT_DIR)/depend

PROGRAM := $(PROGRAM_DIR)/$(PROGRAM_NAME)
SOURCE_NAMES = $(notdir $(SOURCES))
OBJS := $(addprefix $(OBJ_DIR)/,$(SOURCE_NAMES:.cpp=.o))
DEPENDS := $(addprefix $(DEPEND_DIR)/,$(SOURCE_NAMES:.cpp=.depend))

.PHONY: all
all: $(DEPENDS) $(PROGRAM)

$(PROGRAM): $(OBJS)
	@$(call MKDIR,$(PROGRAM_DIR))
	$(CXX) $(CXXFLAGS) $(LIBS) $^ -o $(PROGRAM)

$(DEPEND_DIR)/%.depend: $(SOURCE_DIR)/%.cpp $(HEADERS)
	@echo "generating $@"
	@$(call MKDIR,$(DEPEND_DIR))
	@$(CXX) $(CXXFLAGS) $(LIBS) -I$(HEADER_DIR) -MT "$(OBJ_DIR)/$*.o $@" -MM $< > $@

$(OBJ_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	@$(call MKDIR,$(OBJ_DIR))
	$(CXX) $(CXXFLAGS) $(LIBS) -I$(HEADER_DIR) -c $< -o $@

ifneq "$(MAKECMDGOALS)" "clean"
-include $(DEPENDS)
endif

.PHONY : clean
clean:
	$(RM) -r $(OUT_DIR)

