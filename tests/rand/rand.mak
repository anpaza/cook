TESTS += trand
DESCRIPTION.trand = Проверка псевдослучайного генератора случайных чисел
TARGETS.trand = trand$E
SRC.trand$E = $(wildcard tests/rand/*.c) \
    $(filter-out %/diehard.c,$(wildcard tests/rand/diehard/*.c))
LIBS.trand += useful$L
