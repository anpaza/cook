TESTS += tvector
DESCRIPTION.tvector = Проверка функций работы с векторами
TARGETS.tvector = tvector$E
SRC.tvector$E = $(wildcard tests/vector/*.c)
LIBS.tvector += useful$L
