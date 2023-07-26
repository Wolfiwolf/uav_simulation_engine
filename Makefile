
BLD_DIR=build
SRC_DIR=src

SRCS=$(shell find $(SRC_DIR) -name "*.cpp")
INCS=$(shell find $(SRC_DIR) -name "*.hpp")
OBJS=$(SRCS:%=$(BLD_DIR)/%.o)
TARGET=uav_simulation_engine

$(TARGET): $(OBJS)
	g++ -o $(TARGET) $(OBJS) 

$(BLD_DIR)/%.cpp.o: %.cpp $(INCS)
	@mkdir -p $(@D)
	g++ -g -c $< -o $@  -lpthread -lm -std=c++11

clean:
	rm -rf $(BLD_DIR)
	rm $(TARGET)

show:
	@echo $(OBJS)

