#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct flags {
  int e, i, v, c, l, n, h, s, f, o;
  char pattern[2048];
  int len_pattern;
} flags;

void print_match(regex_t *re, char *line);
void pattern_add(flags *flag, const char *pattern);
void add_reg_from_file(flags *flag, const char *filepath);
flags parser(int argc, char *argv[]);
void process_file(flags flag, const char *path, regex_t *reg);
void output(flags arg, int argc, char **argv);
void output_line(const char *str, int n);

int main(int argc, char **argv) {
  flags flag = parser(argc, argv);
  output(flag, argc, argv);
  return 0;
}

void pattern_add(flags *flag, const char *pattern) {
  if (flag->len_pattern != 0) {
    strcat(flag->pattern + flag->len_pattern, "|");
    flag->len_pattern++;
  }
  flag->len_pattern +=
      sprintf(flag->pattern + flag->len_pattern, "(%s)", pattern);
}

void add_reg_from_file(flags *flag, const char *filepath) {
  FILE *file = fopen(filepath, "r");
  if (file == NULL) {
    if (!flag->s) perror(filepath);
    exit(1);
  }
  char *str = NULL;
  size_t memory = 0;
  int read = getline(&str, &memory, file);
  while (read != -1) {
    if (str[read - 1] == '\n') str[read - 1] = '\0';
    pattern_add(flag, str);
    read = getline(&str, &memory, file);
  }
  free(str);
  fclose(file);
}

flags parser(int argc, char *argv[]) {
  flags flag = {0};
  int res;
  while ((res = getopt(argc, argv, "e:ivclnhsf:o")) != -1) {
    switch (res) {
      case 'e':
        flag.e = 1;
        pattern_add(&flag, optarg);
        break;
      case 'i':
        flag.i = REG_ICASE;
        break;
      case 'v':
        flag.v = 1;
        break;
      case 'c':
        flag.c = 1;
        break;
      case 'l':
        flag.c = 1;
        flag.l = 1;
        break;
      case 'n':
        flag.n = 1;
        break;
      case 'h':
        flag.h = 1;
        break;
      case 's':
        flag.s = 1;
        break;
      case 'f':
        flag.f = 1;
        add_reg_from_file(&flag, optarg);
        break;
      case 'o':
        flag.o = 1;
        break;
    }
  }
  if (flag.len_pattern == 0) {
    pattern_add(&flag, argv[optind]);
    optind++;
  }
  if (argc - optind == 1) {
    flag.h = 1;
  }
  return flag;
}

void output_line(const char *str, int n) {
  for (int i = 0; i < n; i++) {
    putchar(str[i]);
  }
  if (str[n - 1] != '\n') putchar('\n');
}

void process_file(flags flag, const char *path, regex_t *reg) {
  FILE *file = fopen(path, "r");
  if (file == NULL) {
    if (!flag.s) perror(path);
    exit(1);
  }
  char *line = NULL;
  size_t memory = 0;
  int read = 0;
  int line_count = 1;
  int count = 0;
  read = getline(&line, &memory, file);
  while (read != -1) {
    int result = regexec(reg, line, 0, NULL, 0);

    if ((result == 0 && !flag.v) || (flag.v && result != 0)) {
      if (!flag.c && !flag.l) {
        if (!flag.h) printf("%s:", path);
        if (flag.n) printf("%d:", line_count);
        if (flag.o) {
          print_match(reg, line);
        } else {
          output_line(line, read);
        }
      }
      count++;
    }
    read = getline(&line, &memory, file);
    line_count++;
  }
  free(line);
  if (flag.c && !flag.l) {
    if (!flag.h) printf("%s:", path);
    printf("%d\n", count);
  }
  if (flag.l && count > 0) printf("%s\n", path);
  fclose(file);
}

void output(flags flag, int argc, char **argv) {
  regex_t re;
  int error = regcomp(&re, flag.pattern, REG_EXTENDED | flag.i);
  if (error) perror("Error");
  for (int i = optind; i < argc; i++) {
    process_file(flag, argv[i], &re);
  }
  regfree(&re);
}

void print_match(regex_t *re, char *str) {
  regmatch_t match;
  int offset = 0;
  while (1) {
    int result = regexec(re, str + offset, 1, &match, 0);

    if (result != 0) {
      break;
    }

    for (int i = match.rm_so; i < match.rm_eo; i++) {
      putchar(str[offset + i]);
    }
    putchar('\n');
    offset += match.rm_eo;
  }
}