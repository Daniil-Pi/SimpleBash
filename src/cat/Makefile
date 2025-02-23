CC = gcc
CFLAGS = -Wall -Werror -Wextra
FILE = test.txt

all: s21_cat test clangi cpp

s21_cat: s21_cat.c
	$(CC) $(CFLAGS) s21_cat.c -o s21_cat

cpp: 
	cppcheck --enable=all --suppress=missingIncludeSystem ./

test:
	sh test.sh

clean:
	rm -rf s21_cat