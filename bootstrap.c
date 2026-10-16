#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define output printf

enum {
    TOK_KW_CONST,
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
    "CONST",
    "VAR",
    "PROCEDURE",
    "BEGIN",
    "END",
    "IF",
    "THEN",
    "WHILE",
    "DO",
    "CALL",
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
    } idens;

} pool = {0};

struct {
    struct {
        int cat;
        int val;
    } tok;
    char buf[4096];
    char *src;
    int line;
} lexr = {0};

void err() {
    fprintf(stderr, "Error on line %d\n"
        "%s"
        "at:\n"
        "%s"
    , lexr.line, lexr.buf, lexr.src);
    exit(EXIT_FAILURE);
} 

void lexr_next() {
    for (;;) {
        if (!*lexr.src) {
            lexr.tok.cat = TOK_EOF;
            return;
        }
        else if (*lexr.src == '\n') {
            lexr.src = fgets(lexr.buf, 4096, stdin);
            ++lexr.line;
        }
        else if (isspace(*lexr.src)) {
            ++lexr.src;
        }
        else if (isdigit(*lexr.src)) {
            lexr.tok.cat = TOK_NUM;
            lexr.tok.val = strtol(lexr.src, &lexr.src, 0);
            return;
        }
        else {
            int i, j;
            for (i = 0; TOKS[i]; ++i) {
                j = strlen(TOKS[i]);
                if (!memcmp(lexr.src, TOKS[i], j) &&
                    (!isalnum(*lexr.src) || !isalnum(lexr.src[j])))
                {
                    lexr.tok.cat = i;
                    lexr.src += j;
                    return;
                }
            }
            i = 0;
            while (isalnum(lexr.src[++i]));
            lexr.tok.cat = TOK_IDEN;
            for (j = 0; j < pool.idens.len; ++j) {
                if (strlen(pool.idens.val[j]) == i && !memcmp(lexr.src, pool.idens.val[j], i)) {
                    lexr.tok.val = j;
                    lexr.src += i;
                    return;
                }
            }

            j = pool.idens.len;

            pool.idens.val = realloc(pool.idens.val, ++pool.idens.len);
            pool.idens.val[j] = malloc(i + 1);

            memcpy(pool.idens.val[j], lexr.src, i);
            pool.idens.val[j][i] = '\0';

            lexr.tok.val = j;

            lexr.src += i;
            return;
        }
    }
}

void lexr_kickstart() {
    lexr.buf[0] = '\n';
    lexr.src = lexr.buf;
    lexr_next();
}

void pars_const() {
    lexr_next();
    exit(EXIT_SUCCESS);
}

void pars_proc() {
    lexr_next();
    while (lexr.tok.cat == TOK_KW_CONST) {
        pars_const();
    }
}

void pars_code() {
    output(
        ".code64"
        ".rodata"
    );
}

int main() {
    lexr_kickstart();
    while (lexr.tok.cat != TOK_PERIOD) {
        pars_code();
    }
}
