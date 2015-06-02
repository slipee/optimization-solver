CPP_FILES := $(wildcard src/*.cpp)

optimization.out: $(CPP_FILES)
ifeq (“$(CPL)”, “mpixlcxx”)
	mpixlcxx $^ -o $@ -O2
else
	mpicxx $^ -o $@ -O2
endif