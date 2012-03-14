all: test

build:
	flex src/json.l
	bison -d src/json.y 
	gcc -g -o dump -I./src json.tab.c lex.yy.c src/dump.c src/json.c
	gcc -g -o translate -I./src json.tab.c lex.yy.c src/tosvg.c src/json.c
	#remove temporary files
	rm -f json.tab.h json.tab.c lex.yy.c

test: build
	./translate < ./tests/reg_cascade.json 2>&1 > reg_cascade.svg
	./translate < ./tests/top_level.json 2>&1 > top_level.svg 

clean:
	#remove tests
	rm -f reg_cascade.svg top_level.svg yacc.log
	#remove binaries
	rm -f dump translate 
