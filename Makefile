
ifdef USE_INT
MACRO = -DUSE_INT
endif

#compiler setup
CXX = g++
MPICXX = mpic++
CXXFLAGS = -std=c++14 -O3 $(MACRO)

COMMON= core/utils.h core/cxxopts.h core/get_time.h 
SERIAL= knapsack_serial create_file
PARALLEL= knapsack_parallel
DISTRIBUTED= knapsack_distributed
ALL= $(SERIAL) $(PARALLEL) $(DISTRIBUTED)

all : $(ALL)

$(SERIAL): %: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

$(PARALLEL): %: %.cpp
	$(CXX) $(CXXFLAGS) -pthread -o $@ $<

$(DISTRIBUTED): %: %.cpp
	$(MPICXX) $(CXXFLAGS) -o $@ $<

.PHONY : clean

clean :
	rm -f *.o *.obj $(ALL)
