# Quokka

## TODO

- All parts following parser
- Conditionals

## Lexer

Parses the string of the input file and tokenizes it into the following sets

```
IDENTIFIER : {a-z}+
NUMBER     : {0..9}+
OPERATOR   : {+,-,*,/}
ASSIGNMENT : {<-}
SEMI-COLON : {;}
```

## Parser

Defined by the following context free grammar

```
<program>    ::= <statement> | <statement> <program>
<statement>  ::= <identifier> ASSIGNMENT <expression> SEMI-COLON
<expression> ::= <term> | <term> OPERATOR <expression>
<term>       ::= IDENTIFIER | NUMBER 
```

## Transpiler 

Converts to C code