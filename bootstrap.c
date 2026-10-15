#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

enum {
    TOK_KW_VAR,
    TOK_KW_PROC,
    TOK_KW_BEG,
    TOK_KW_END,
    TOK_KW_IF,
    TOK_KW_THEN,
    TOK_KW_WHILE,
    TOK_KW_DO,
    TOK_KW_CALL,
    
    TOK_PERIOD,
    TOK_EQU,
    TOK_COMMA,
    TOK_SEMI,
    TOK_ASSIGN,
    TOK_QUEST,
    TOK_EXCL,
    TOK_HASH,
    TOK_LESS,
    TOK_LEQ,
    TOK_GRTR,
    TOK_GEQ,
    TOK_ADD,
    TOK_SUB,
    TOK_MUL,
    TOK_DIV,
    TOK_PAREN_L,
    TOK_PAREN_R,

    TOK_IDEN,
    TOK_NUM,
    TOK_EOF,
};

char const * const TOKS[] = {
    "var",
    "proc",
    "beg",
    "end",
    "if",
    "then",
    "while",
    "do",
    "call",
    ".",
    "=",
    ",",
    ";",
    ":=",
    "?",
    "!",
    "#",
    "<",
    "<=",
    ">",
    ">=",
    "+",
    "-",
    "*",
    "/",
    "(",
    ")",
    NULL
};

struct {
    struct {
        char **val;
        int len;
    } idents;

} pool;

struct {
    struct {
        int cat;
        int val;
    } tok;
    char buf[4096];
    char *src;
} lexr;

void lexr_next() {
for (;;) {
    if (!*lexr.src) {
        lexr.src = fgets(lexr.buf, 4096, stdin);
    }
    else if (isdigit(*lexr.src)) {
        lexr.tok.cat = TOK_NUM;
        lexr.tok.val = strtol(lexr.src, &lexr.src, 0);
    }
    else {
        int i;
        for (i = 0; TOKS[i]; ++i) {
            int len;
            len = strlen(TOKS[i]);
        }
    }
}
}

void lexr_kickstart() {
    lexr.buf[0] = '\0';
    lexr.src = lexr.buf;
    lexr_next();
}


void pars_code() {
    switch (lexr.tok.cat) {
    case TOK_KW_PROG
    }
}

int main() {
    lexr_kickstart();
    while (lexr.tok.cat != TOK_PERIOD) {
        pars_code();
    }
}
