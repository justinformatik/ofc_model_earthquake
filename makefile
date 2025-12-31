BINARY    := earthquake
HEADER    := my_numerics
MT_BINARY := mersennetwister
L_FLAGS   := -O3 -Wall -lm -lgsl -lgslcblas

HEADER_OBJ := $(HEADER).o
BINARY_SRC := $(BINARY).c
MT_OBJ     := $(MT_BINARY).o
MT_SRC     := $(MT_BINARY).c


$(BINARY): $(BINARY_SRC) $(HEADER_OBJ)
	gcc -o $@ $^ $(L_FLAGS)

$(HEADER_OBJ): $(HEADER).c
	gcc -c $< -o $@ $(L_FLAGS)

run: $(BINARY)
	./$(BINARY)


$(MT_OBJ): $(MT_SRC)
	gcc -c $< -o $@ $(L_FLAGS)

compileMT: $(MT_OBJ) $(HEADER_OBJ)
	gcc -o $(MT_BINARY) $^ $(L_FLAGS)


runMT: compileMT
	./$(MT_BINARY)


plot:
	python3 1.py
	python3 2plot.py
	python3 2mean.py
	python3 3.py
	python3 2tail.py 	
	python3 1ani.py 
plotMT:
	python3 mersennetwister.py


clean:
	rm -f $(BINARY) $(MT_BINARY) $(HEADER_OBJ) $(MT_OBJ) *.o *.csv *.dat *.txt


debugCompile: $(BINARY_SRC) $(HEADER_OBJ)
	gcc -o $(BINARY) $^ -fsanitize=address -g $(L_FLAGS)


.PHONY: run runMT plot plotMT clean debug

all: clean $(BINARY) run plot

mt: compileMT runMT plotMT 

debug: debugCompile run clean
