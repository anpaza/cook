TESTS += tparser
DESCRIPTION.tparser = Проверка синтаксического анализатора
TARGETS.tparser = tparser$E
SRC.tparser$E = $(wildcard tests/parser/*.c)
LIBS.tparser += cooker$L useful$L
