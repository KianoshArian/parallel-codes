all: src/q1/result.out src/q2/result.out src/q3/result.out src/q4/result.out

%/main.out: %/main.cpp
	g++ $< -o $@

%/result.out: %/main.out
	cd $* && ./main.out > ./result.out

clean: 
	rm -f src/*/*.out
