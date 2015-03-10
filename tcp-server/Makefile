#------------------------------------------------------------------------
#	Set the Right Directories
#------------------------------------------------------------------------
DIR_OBJ = ./obj
DIR_BIN = ./bin
DIR_INC = ./inc
DIR_SRC = ./src

#------------------------------------------------------------------------
#	Set the cxxflags and dependent libs
#------------------------------------------------------------------------
CXXFLAGS = -O2 -g -Wall -fmessage-length=0 -std=c++1y -I$(DIR_INC)
LIBS = opencv_core opencv_highgui pthread

#------------------------------------------------------------------------
#	Set the objective bin names and dependences
#------------------------------------------------------------------------
SRV_OBJ = ir_comm.o ir_server.o srv_main.o
SRV_BIN =	ir_server

CLI_OBJ = ir_comm.o ir_client.o
CLI_BIN = ir_client

#------------------------------------------------------------------------
#	Process the Dirs
#------------------------------------------------------------------------
LIBS := $(LIBS:%=-l%)
DIRS := $(DIR_OBJ) $(DIR_BIN)

SRV_OBJ := $(SRV_OBJ:%=$(DIR_OBJ)/%)
SRV_BIN := $(SRV_BIN:%=$(DIR_BIN)/%)

CLI_OBJ := $(CLI_OBJ:%=$(DIR_OBJ)/%)
CLI_BIN := $(CLI_BIN:%=$(DIR_BIN)/%)

#------------------------------------------------------------------------
#	Target
#------------------------------------------------------------------------
all: clean $(DIRS) $(SRV_BIN) $(CLI_BIN)

$(DIRS):
	mkdir $@ 

$(SRV_BIN): $(SRV_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)
	
$(CLI_BIN): $(CLI_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

$(DIR_OBJ)/%.o: $(DIR_SRC)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $^

#------------------------------------------------------------------------
#	Clean
#------------------------------------------------------------------------
clean:
	rm -f $(DIR_OBJ)/*.o $(DIR_BIN)/*
.PHONY: clean
