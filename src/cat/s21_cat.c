#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct flags {
  int b, n, s, E, T, v;
} flags;

flags parser(int argc, char **argv);
void s21_cat(int argc, char **argv, flags *flag);

int main(int argc, char *argv[]) {
  flags flag = parser(argc, argv);
  s21_cat(argc, argv, &flag);
  return 0;
}

flags parser(int argc, char **argv) {
  flags flag = {0};

  struct option long_options[] = {{"number-nonblank", no_argument, NULL, 'b'},
                                  {"number", no_argument, NULL, 'n'},
                                  {"squeeze-blank", no_argument, NULL, 's'},
                                  {0, 0, 0, 0}};
  int res;
  while ((res = getopt_long(argc, argv, "bneETtsv", long_options, 0)) != -1) {
    switch (res) {
      case 'b':
        flag.b = 1;
        break;
      case 'n':
        flag.n = 1;
        break;
      case 's':
        flag.s = 1;
        break;
      case 'E':
        flag.E = 1;
        break;
      case 'e':
        flag.E = 1;
        flag.v = 1;
        break;
      case 'T':
        flag.T = 1;
        break;
      case 't':
        flag.T = 1;
        flag.v = 1;
        break;
      case 'v':
        flag.v = 1;
        break;
      case '?':
        perror("ERROR");
        break;
      default:
        break;
    }
  }
  return flag;
}

void s21_cat(int argc, char **argv, flags *flag) {
  FILE *fp;

  if (optind == argc)
    fp = stdin;
  else
    fp = fopen(argv[optind], "r");

  if (fp != NULL) {
    int c, pred = '\n', pred2 = 0, count = 1;
    while ((c = fgetc(fp)) != EOF) {
      int q = 0;
      if (flag->s && c == '\n' && pred == '\n' && pred2 == '\n') continue;
      if (flag->b && pred == '\n' && c != '\n') {
        printf("%6d\t", count++);
        flag->n = 0;
      }
      if (flag->n && pred == '\n') printf("%6d\t", count++);
      if (flag->E && c == '\n') printf("$");
      if (flag->T && c == '\t') {
        q = 1;
        c = 'I';
      }
      if (flag->v) {
        if (c >= 128 && c <= 159) {
          c = c & 0x7f;
          printf("M-");
        }
        if (c >= 0 && c <= 31 && c != '\n' && c != '\t') {
          c += 64;
          q = 1;
        }
        if (c == 127) {
          c = '?';
          q = 1;
        }
      }
      if (q) printf("^");
      printf("%c", c);
      pred2 = pred;
      pred = c;
    }
    fclose(fp);
  } else
    fprintf(stderr, "No such file or directory");
}