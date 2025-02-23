CC = gcc
CFLAGS = -Wall -Werror -Wextra
FILE = test.txt

all: s21_grep

s21_grep: s21_grep.c
	$(CC) $(CFLAGS) s21_grep.c -o s21_grep

cpp:
	cppcheck --enable=all --suppress=missingIncludeSystem ./

test:
	sh test.sh

clean:
	rm -rf s21_grep