/**
 * digitgroup
 * uses ANSI colouring to split large numbers into nice thousands groups
 * e.g.,     1234567890
 *           ^   ^^^    <-- those digits are highlighted
 *            
 * USAGE: PSQL_PAGER="digitgroup | less -RXF" psql -P pager=always -h mydb -U postgres postgres
 * or to see what it does:
 *   echo 1234567890 | digitgroup
 *
 * likely you'll want to add this in your .zshrc:
 *     alias psql='PSQL_PAGER="/Users/MYNAME/bin/digitgroup|less -RXF" psql -P pager=always'
 * BUILDING:
 *  gcc digitgroup.c -O2 -o digitgroup
 *
 * TODO:
 *  - be smarter about what NOT to highlight (not harmful, but possibly annoying)
 *    - decimals:   3.14159265  <-- don't need to colourize those decimal digits
 *    - "e" notation: 1.8446744073709552e+19  <-- maybe handled by decimal case above
 *    - UUIDs
 *    - Hexidecimal
 *  - easiest way might be to add another state STATE_SKIPPING_PAST_CHARS which is set when the characture is
 *    - not a space, dollar sign (or other currency), parenthesis, bracket, etc.
 *    - i.e., when it's alpha, underscore, dot, comma (eu)
*/
#include <stdio.h>
#include <string.h>

#define MAX_BUFFER 16384             // supports lines up to this length

#define END_COLOR_GROUP "\033[0m"    // reset

enum {
  STATE_LOOKING_FOR_DIGIT,
  STATE_IN_NUMBER
};

void print_number(char *buffer, int len, char *start_highlight) {
  if ((len % 6) > 3) {
    fwrite(start_highlight, 1, strlen(start_highlight), stdout);
  }
  
  while (len-- > 0) {
    if ((len % 6) == 5) {
      fwrite(start_highlight, 1, strlen(start_highlight), stdout);
    }
    putc(*buffer++, stdout);
    if ((len % 6) == 3) {
      fwrite(END_COLOR_GROUP, 1, strlen(END_COLOR_GROUP), stdout);
    }
  }
}

// TODO: should we only work on numbers that are separated by word breaks? (non-letters)
//  e.g., don't format hex: ABC123456789F?
int main(int argc, char **argv) {
  char source_buffer[MAX_BUFFER];
  char *number_start;
  char *text_start;
  char *c;
  int state;

  char *highlight_code = "31"; // highlight in red by default

  if (argc > 1 && strlen(argv[1]) < 5) highlight_code = argv[1]; // let the user pass in an alternative
  
  char start_highlight[20];
  sprintf(start_highlight, "\033[%sm", highlight_code);

  while (fgets(source_buffer, MAX_BUFFER, stdin) ) {
    state = STATE_LOOKING_FOR_DIGIT;
    text_start = source_buffer;
    for (c = source_buffer; *c != 0; c++) {
      switch (state) {
        case STATE_LOOKING_FOR_DIGIT:
          if (*c >= '0' && *c <= '9') {
            if (c > text_start) {
              fwrite(text_start, 1, c - text_start, stdout);
            }
            state = STATE_IN_NUMBER;
            number_start = c;
          }
          break;

        case STATE_IN_NUMBER:
          if (*c < '0' || *c > '9') {
            state = STATE_LOOKING_FOR_DIGIT;
            print_number(number_start, c - number_start, start_highlight);
            text_start = c;
          }
          break;
      }
    }

    if (state == STATE_IN_NUMBER) {
      print_number(number_start, c - number_start, start_highlight);
    }
    else if (c > text_start) {
      fwrite(text_start, 1, c - text_start, stdout);
    }
  }
  
  return 0;
}
