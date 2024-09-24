# digitgroup
ANSI-Colourize groups of three digits printed in the terminal for easier reading

e.g.,

`echo 1234567890 | digitgroup`

$${\texttt{\color{red}1\color{black}234\color{red}567\color{black}890}}$$

# USAGE
```
PSQL_PAGER="digitgroup | less -RXF" psql -P pager=always -h mydb -U postgres postgres
```

or to see what it does:
```
  echo 1234567890 | digitgroup
```

likely you'll want to add this in your .zshrc:
```
    alias psql='PSQL_PAGER="/Users/MYNAME/bin/digitgroup|less -RXF" psql -P pager=always'
```

# BUILDING:
```
 gcc digitgroup.c -O2 -o digitgroup
 mv digitgroup ~/bin   # or wherever you like to keep your custom executables
```


# TODO
 - be smarter about what NOT to highlight (not harmful, but possibly annoying)
   - decimals:   3.14159265  <-- don't need to colourize those decimal digits
   - "e" notation: 1.8446744073709552e+19  <-- maybe handled by decimal case above
   - UUIDs
   - Hexidecimal
 - easiest way might be to add another state STATE_SKIPPING_PAST_CHARS which is set when the characture is
   - not a space, dollar sign (or other currency), parenthesis, bracket, etc.
   - i.e., when it's alpha, underscore, dot, comma (eu)
