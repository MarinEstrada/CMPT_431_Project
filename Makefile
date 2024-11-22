distributed: knapsack_distributed.cpp
	mpic++ -std=c++14 -O3 -g -o knapsack_distributed knapsack_distributed.cpp

clean:
	rm -f knapsack_distributed