#include <cassert>
#include <cstdio>

bool matchhere(char *, char *);
bool matchstar(int, char *, char *);

/* matchstar: search for c*regexp at beginning of text */
bool matchstar(int c, char *regexp, char *text) {
  do { /* a * matches zero or more instances */
    if (matchhere(regexp, text))
      return true;
  } while (*text != '\0' && (*text++ == c || c == '.'));
  return false;
}

/* match: search for regexp anywhere in text */
bool match(char *regexp, char *text) {
  if (regexp[0] == '^')
    return matchhere(regexp + 1, text);
  do { /* must look even if string is empty */
    if (matchhere(regexp, text))
      return true;
  } while (*text++ != '\0');
  return false;
}

/* matchhere: search for regexp at beginning of text */
bool matchhere(char *regexp, char *text) {
  if (regexp[0] == '\0')
    return true;
  if (regexp[1] == '*')
    return matchstar(regexp[0], regexp + 2, text);
  if (regexp[0] == '$' && regexp[1] == '\0')
    return *text == '\0';
  if (*text != '\0' && (regexp[0] == '.' || regexp[0] == *text))
    return matchhere(regexp + 1, text + 1);
  return false;
}

int main(int argc, char **argv) {
  assert(argc == 3);

  printf("%d\n", argc);
  printf("%s\n", argv[1]);
  printf("%s\n", argv[2]);

  if (match(argv[1], argv[2])) {
    printf("match\n");
    return 0;
  } else {
    printf("no match\n");
    return 1;
  }
}