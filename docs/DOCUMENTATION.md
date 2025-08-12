# Quokka Documentation

## TODO:

*See README for up to date features to be implemented*

## Lexer

Parses the string of the input file and tokenizes it into the following sets

```
IDENTIFIER : {a-z}+
NUMBER     : {0..9}+
OPERATOR   : {+,-,*,/}
ASSIGNMENT : {=}
SEMI-COLON : {;}
```

## Parser

Builds an Abstract Syntax Tree (AST) using recursive descent parsing.  

Defined by the following context free grammar:

```
<program>    ::= <statement> | <statement> <program>
<statement>  ::= <identifier> ASSIGNMENT <expression> SEMI-COLON
<expression> ::= <term> | <term> OPERATOR <expression>
<term>       ::= IDENTIFIER | NUMBER 
```

## Evaluator

Evaluates the AST to "run" the program itself.