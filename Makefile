OBJS = clhelp.o opencl_setup.o opencl_setup_harness.o wrapper.o
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S), Linux)
OCL_INC=/usr/local/cuda-4.2/include
OCL_LIB=/usr/local/cuda-4.2/lib64

%.o: %.cpp clhelp.h
	g++ -msse4 -O2 -c $< -I$(OCL_INC)

all: $(OBJS)
	g++  opencl_setup.cpp clhelp.cpp wrapper.cpp -o wrapper -I$(OCL_INC) -L$(OCL_LIB) -lOpenCL -fopenmp
#	g++  *.cpp -L$(OCL_LIB) -lOpenCL
#/	g++ opencl_setup_harness.o opencl_setup.o clhelp.o -o opencl_setup_harness -L$(OCL_LIB) -lOpenCL

endif

clean:
	rm -rf $(OBJS) opencl_setup opencl_setup_harness wrapper
