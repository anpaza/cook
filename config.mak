# Конфигурационный файл системы автоматической сборки для данного проекта

# Режим сборки по умолчанию
MODE = release

# Пара вспомогательных функций
asciiup = $(shell echo $1 | tr a-z A-Z)
asciidown = $(shell echo $1 | tr A-Z a-z)

# Попытка автоопределения текущей ОС (по наличию переменной окружения ComSpec)
ifndef HOST
ifneq ($(if $(ComSpec),$(ComSpec),$(COMSPEC)),)
HOST = win32
else
HOST = $(call asciidown,$(shell uname -s))
endif
endif

# Проверка на установленные соответствующие утилиты
MAKEDEP = $(shell which makedep 2>/dev/null)
DOXYGEN = $(shell which doxygen 2>/dev/null)

# Автоматически обновляем зависимости
ifneq ($(MAKEDEP),)
AUTODEP = 1
endif

# Определим макросы для архитектуры
CFLAGS.DEF += -DARCH_$(call asciiup,$(ARCH))

# Local user file which can be used to override some of the settings (e.g. MODE)
-include local-config.mak
