CXX = g++
MPICXX = mpic++
CXXFLAGS = -std=c++14

COMMON= core/utils.h core/cxxopts.h core/get_time.h
SERIAL= knapsack_serial
PARALLEL= knapsack_parallel
DISTRIBUTED= knapsack_distributed
ALL= $(SERIAL) $(PARALLEL) $(DISTRIBUTED)

all: $(ALL)

$(SERIAL): %: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

$(PARALLEL): %: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

$(DISTRIBUTED): %: %.cpp
	$(MPICXX) $(CXXFLAGS) -o $@ $<

.PHONY : clean

clean :
	rm -r *.o *.obj $(ALL)