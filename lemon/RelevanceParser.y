%token_type {int}

relevance(A)       ::= expression(B).                                          { A = B; }

primary(A)         ::= LPAREN expression(B) RPAREN.                            { A = B; }
primary(A)         ::= STRING(B).                                              { A = B; }
primary(A)         ::= NUMBER(B).                                              { A = B; }
primary(A)         ::= IT(B).                                                  { A = B; }

index(A)           ::= PHRASE(B) primary(C).                                   { A = B + C; }
index(A)           ::= PHRASE(B).                                              { A = B; }
index(A)           ::= primary(B).                                             { A = B; }

property(A)        ::= property(B) WHOSE primary(C).                           { A = B + C; }
property(A)        ::= index(B).                                               { A = B; }

reference(A)       ::= property(B) OF reference(C).                            { A = B + C; }
reference(A)       ::= property(B).                                            { A = B; }

cast(A)            ::= cast(B) AS phrase(C).                                   { A = B + C; }
cast(A)            ::= reference(B).                                           { A = B; }

unary(A)           ::= EXISTS    unary(B).                                     { A = B; }
unary(A)           ::= NOTEXISTS unary(B).                                     { A = B; }
unary(A)           ::= NOT       unary(B).                                     { A = B; }
unary(A)           ::= MINUS     unary(B).                                     { A = B; }
unary(A)           ::= cast(B).                                                { A = B; }

product(A)         ::= product(B) TIMES  unary(C).                             { A = B + C; }
product(A)         ::= product(B) DIVIDE unary(C).                             { A = B + C; }
product(A)         ::= product(B) MOD    unary(C).                             { A = B + C; }
product(A)         ::= product(B) CONCAT unary(C).                             { A = B + C; }
product(A)         ::= unary(B).                                               { A = B; }

sum(A)             ::= sum(B) PLUS  product(C).                                { A = B + C; }
sum(A)             ::= sum(B) MINUS product(C).                                { A = B + C; }
sum(A)             ::= product(B).                                             { A = B; }

relationExpr(A)    ::= sum(B) RELATION sum(C).                                 { A = B + C; }
relationExpr(A)    ::= sum(B).                                                 { A = B; }

andExpression(A)   ::= andExpression(B) AND relationExpr(C).                   { A = B + C; }
andExpression(A)   ::= relationExpr(B).                                        { A = B; }

orExpression(A)    ::= orExpression(B) OR andExpression(C).                    { A = B + C; }
orExpression(A)    ::= andExpression(B).                                       { A = B; }

tuple(A)           ::= orExpression(B) COMMA tuple(C).                         { A = B + C; }
tuple(A)           ::= orExpression(B).                                        { A = B; }

collection(A)      ::= collection(B) SEMICOLON tuple(C).                       { A = B + C; }
collection(A)      ::= tuple(B).                                               { A = B; }

expression(A)      ::= IF expression(B) THEN expression(C) ELSE expression(D). { A = B + C + D; }
expression(A)      ::= collection(B).                                          { A = B; }
