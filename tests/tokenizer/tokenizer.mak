TESTS += ttokenizer
DESCRIPTION.ttokenizer = Проверка токенизатора
TARGETS.ttokenizer = ttokenizer$E
SRC.ttokenizer$E = $(wildcard tests/tokenizer/*.c)
LIBS.ttokenizer += cooker$L useful$L
