%token_type {int}

%left SEMICOLON.
%right COMMA.
%left OR.
%left AND.
%nonassoc RELATION.
%left PLUS MINUS.
%left TIMES DIVIDE MOD CONCAT.
%left EXISTS NOTEXISTS NOT.
%left AS.
%right OF.

relevance(A) ::= expr(B). { A = B; }

expr(A) ::= IF relation(B) THEN relation(C) ELSE relation(D). { A = B + C + D; }
expr    ::= relation.

relation(A) ::= relation(B) SEMICOLON relation(C). { A = B + C; }
relation(A) ::= relation(B) COMMA     relation(C). { A = B + C; }
relation(A) ::= relation(B) OR        relation(C). { A = B + C; }
relation(A) ::= relation(B) AND       relation(C). { A = B + C; }

relation(A) ::= binary(B) RELATION binary(C). { A = B + C; }
relation    ::= binary.

binary(A) ::= binary(B) PLUS   binary(C). { A = B + C; }
binary(A) ::= binary(B) MINUS  binary(C). { A = B + C; }
binary(A) ::= binary(B) TIMES  binary(C). { A = B + C; }
binary(A) ::= binary(B) DIVIDE binary(C). { A = B + C; }
binary(A) ::= binary(B) MOD    binary(C). { A = B + C; }
binary(A) ::= binary(B) CONCAT binary(C). { A = B + C; }
binary    ::= unary.

unary(A) ::= EXISTS    unary(B). { A = B; }
unary(A) ::= NOTEXISTS unary(B). { A = B; }
unary(A) ::= NOT       unary(B). { A = B; }
unary(A) ::= MINUS     unary(B). { A = B; }
unary    ::= cast.

cast(A) ::= cast(B) AS PHRASE(C). { A = B + C; }
cast    ::= reference.

reference(A) ::= property(B) OF reference(C). { A = B + C; }
reference    ::= property.

property(A) ::= property(B) WHOSE primary(C). { A = B + C; }
property(A) ::= PHRASE(B) primary(C).         { A = B + C; }
property(A) ::= PHRASE(B).                    { A = B; }
property    ::= primary.

primary(A) ::= STRING(B).             { A = B; }
primary(A) ::= NUMBER(B).             { A = B; }
primary(A) ::= IT(B).                 { A = B; }
primary(A) ::= LPAREN expr(B) RPAREN. { A = B; }
