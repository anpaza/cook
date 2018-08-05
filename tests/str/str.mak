TESTS += tstr
DESCRIPTION.tstr = Проверка функций для работы со строками
TARGETS.tstr = tstr$E
SRC.tstr$E = $(wildcard tests/str/*.c)
LIBS.tstr += useful$L
