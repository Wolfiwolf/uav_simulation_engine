
BLD_DIR=build
SRC_DIR=src

SRCS=$(shell find $(SRC_DIR) -name "*.cpp")
SRCS_C=$(shell find $(SRC_DIR) -name "*.c")
INCS=$(shell find $(SRC_DIR) -name "*.hpp")
OBJS=$(SRCS:%=$(BLD_DIR)/%.o)
OBJS_C=$(SRCS_C:%=$(BLD_DIR)/%.o)
TARGET=uav_simulation_engine

$(TARGET): $(OBJS) $(OBJS_C)
	g++ -g -o $(TARGET) $(OBJS) $(OBJS_C)

$(BLD_DIR)/%.cpp.o: %.cpp $(INCS)
	@mkdir -p $(@D)
	g++ -g -c $< -o $@  -lpthread -lm -std=c++11

$(BLD_DIR)/%.c.o: %.c $(INCS)
	@mkdir -p $(@D)
	g++ -g -c $< -o $@  -lpthread -lm -std=c++11

clean:
	rm -rf $(BLD_DIR)
	rm $(TARGET)

show:
	@echo $(OBJS)

