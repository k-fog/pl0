# pl0
This is a PL/0' interpreter.
You can run a code by giving the program as a command line argument. 
This interpreter always outputs a return value without 'write' instruction.

## The syntax of PL/0' in eBNF:
```
<program> ::= <block> '.'
<block> ::=  ( <declaration> )* <statement>
<declaration> ::= <const_decl> | <var_decl> | <func_decl>
<const_decl> ::= 'const' <const_def> ( ',' <const_def> )* ';'
<const_def> ::= ident '=' number
<var_decl> ::= 'var' ident ( ',' ident )* ';'
<func_decl> ::= 'function' ident '(' [ ident ( ',' ident )* ]  ')' <block> ';'
<statement> ::= [ ident ':=' <expression>
             | 'begin' <statement> ( ';' <statement> )* 'end'
             | 'if' <condition> 'then' <statement>
             | 'while' <condition> 'do' <statement>
             | 'return' <expression>
             | 'write' <expression>
             | 'writeln' ]
<condition> ::= 'odd' <expression>
             | <expression> ( '=' | '<>' | '<" | '<=' | '>' | '>=' ) <expression>
<expression> ::= [ ( '-' | '+' ) ] <term> ( ( '-' | '+' ) <term> )*
<term> ::= <factor> ( ( '*' | '/' | '%' ) <factor> )*
<factor> ::= ident '(' [ <expression> ( ',' <expression> )* ] ')'
             | number
             | ident
             | '(' <expression> ')'
```

## Sample programs:

```pascal
var i, x;

begin
    i := 1;
    x := 0;

    while i <= 10 do
    begin
        x := x + i;
        i := i + 1;
    end;
    
    return x;
end.
```

```pascal
function gcd(a,b)
begin
    if a = 0 then return b;
    return gcd(b % a, a);
end;

return gcd(19, 57).
```

```pascal
const N = 10;

function fib(n)
begin
    if n <= 1 then return n;
    return fib(n - 1) + fib(n - 2);
end;

return fib(N).
```
