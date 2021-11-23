TARGET_EXEC := Kartoffeln

BUILD_DIR = ./build
SOURCES = $(shell find . -name '*.cpp')
OBJECTS = $(SOURCES:%=$(BUILD_DIR)/%.o)
CXX = g++
CXXFLAGS = -std=c++11 -fexceptions -O3 -Iinclude
DEPENDENCIES = $(OBJECTS:.o=.d)
CPPFLAGS = -MMD -MP

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@


.PHONY: clean
clean:
	rm -r $(BUILD_DIR)

-include $(DEPENDENCIES)
