%{
    int LINE = 1;

%}

digit   [0-9]
letter  [A-Za-z]
id      (_|{letter})(_|{letter}|{digit})*

%%
if |
else |
for |
while |
printf |
scanf {printf("key words \n");}
{id} {printf("id : %s \n", yytext);}
{num} {printf("a num : %s\n", yytext);}
\n {LINE++;}
[ \t] {}


%%
int main(){
    yylex();
    printf("total lines: %d\n", LINE);
    return 0;
}

int yywrap(){
    return 1;
}
