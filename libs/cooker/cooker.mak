LIBS += cooker
DESCRIPTION.cooker = Интерпретатор языка Cook
TARGETS.cooker = cooker$L
SRC.cooker$L = $(sort $(wildcard libs/cooker/*.c) libs/cooker/cook-parser.c)

libs/cooker/cook-parser.c: libs/cooker/cook-parser.y
	bison -o $@ $<
