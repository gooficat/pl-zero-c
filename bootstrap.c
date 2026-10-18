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
    TOK_KW_ODD,
    
    TOK_PERIOD,
    TOK_ASSIGN,
    TOK_EQU,
    TOK_COMMA,
    TOK_SEMI,
    TOK_QUEST,
    TOK_EXCL,
    TOK_HASH,
    TOK_LEQ,
    TOK_LESS,
    TOK_GEQ,
    TOK_GRTR,
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
    "ODD",
    ".",
    ":=",
    "=",
    ",",
    ";",
    "?",
    "!",
    "#",
    "<=",
    "<",
    ">=",
    ">",
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
}

int disam() {
    static int num = 0;
    return num++;
}

void pars_expr();

void pars_fact() {
    switch (lexr.tok.cat)
    {
    case TOK_IDEN:
        output("\tmov VAR.%s(%%rip), %%rax\n", pool.idens.val[lexr.tok.val]);
        lexr_next();
        break;
    case TOK_NUM:
        output("\tmov $%d, %%rax\n", lexr.tok.val);
        lexr_next();
        break;
    case TOK_PAREN_L:
        lexr_next();
        pars_expr();
        lexr_next();
        break;
    }
}

void pars_term() {
    pars_fact();
    switch (lexr.tok.cat)
    {
    case TOK_MUL:
        output("\tpush %%rax\n");
        lexr_next();
        pars_term();
        output("\tpop %%rbx\n"
               "\timul %%rbx\n");
        break;
    case TOK_DIV:
        output("\tpush %%rax\n");
        lexr_next();
        pars_term();
        output("\tpop %%rbx\n"
               "\tidiv %%rbx\n");
        break;
    default:
        break;
    }
}

void pars_expr() {
    switch (lexr.tok.cat)
    {
    case TOK_SUB:
        pars_term();
        output("\tneg %%rax\n");
    case TOK_ADD:
    default:
        pars_term();
    }
repeat:
    switch (lexr.tok.cat)
    {
    case TOK_SUB:
        output("\tpush %%rax\n");
        lexr_next();
        pars_term();
        output("\tpop %%rbx\n"
               "\tsub %%rbx, %%rax\n"
               "\txchg %%rbx, %%rax");
        break;
    case TOK_ADD:
        output("\tpush %%rax\n");
        lexr_next();
        pars_term();
        output("\tpop %%rbx\n"
               "\tadd %%rbx, %%rax\n");
        break;
    default:
        goto done;
    }
    goto repeat;
done:
    return;
}

void pars_stmt();

void pars_blok() {
printf("/ block\n");
    do {
        lexr_next();
        pars_stmt();
        if (lexr.tok.cat == TOK_KW_END) {
            break;
        }
    } while (lexr.tok.cat == TOK_SEMI);
    lexr_next();
printf("/ end block\n");
}

void pars_cond(int jn) {
    if (lexr.tok.cat == TOK_KW_ODD) {
        lexr_next();
        pars_expr();
        output("\tand $1, %%rax\n"
               "\tcmp $1, %%rax\n"
               "jne %d", jn);
    }
    else {
        int cond;
        pars_expr();
        output("\tpush %%rax\n");
        cond = lexr.tok.cat;
        lexr_next();
        pars_expr();
        output("\tpop %%rbx\n");
        output("\tcmp %%rax, %%rbx\n");

        switch (cond)
        {
        case TOK_EQU:
            output("\tjne JMP.%d\n", jn);
            break;
        case TOK_HASH:
            output("\tje JMP.%d\n", jn);
            break;
        case TOK_LESS:
            output("\tjge JMP.%d\n", jn);
            break;
        case TOK_LEQ:
            output("\tjg JMP.%d\n", jn);
            break;
        case TOK_GRTR:
            output("\tjle JMP.%d\n", jn);
            break;
        case TOK_GEQ:
            output("\tjl JMP.%d\n", jn);
            break;
        }
    }
}

void pars_stmt() {
    switch (lexr.tok.cat)
    {
    case TOK_IDEN: {
        char *name;
        name = pool.idens.val[lexr.tok.val];
        lexr_next();
        lexr_next(); /* := */
        pars_expr();
        output("\tmov %%rax, VAR.%s(%%rip)\n", name);
    }
        break;
    case TOK_KW_CALL:
        lexr_next();
        output("\tcall PROC.%s\n", pool.idens.val[lexr.tok.val]);
        lexr_next();
        break;
    case TOK_QUEST:
        lexr_next();
        output("\tcall pl.getc\n"
               "\tmov %%rax, VAR.%s(%%rip)\n");
        lexr_next();
        break;
    case TOK_EXCL:
        lexr_next();
        pars_expr();
        output("\tcall pl.putc\n");
        break;
    case TOK_KW_BEG:
        pars_blok();
        break;
    case TOK_KW_IF: {
        int false_case;
        false_case = disam();
        pars_cond(false_case);
        pars_stmt();
        output("JMP.%d:\n", false_case);
    }
        break;
    case TOK_KW_WHILE: {
        int repeat, end;
        repeat = disam();
        end = disam();
        lexr_next();
        output("JMP.%d:\n", repeat);
        pars_cond(end);
        lexr_next(); /* DO */
        pars_stmt();
        output("\tjmp JMP.%d\n"
               "JMP.%d:\n", repeat, end);
    }
        break;
    default:
        break;
    }
}

void pars_code() {
    output(
        ".code64\n"
        ".data\n"
        ".globl main\n"
        ".extern pl.getc\n"
        ".extern pl.putc\n"
    );

    while (lexr.tok.cat == TOK_KW_CONST) {
        lexr_next();
        output("\tCONST.%s:", pool.idens.val[lexr.tok.val]);
    }
    output(".bss\n");
    while (lexr.tok.cat == TOK_KW_VAR) {
        lexr_next();
        if (lexr.tok.cat != TOK_IDEN) {
            err();
        }
    add_var:
        output("VAR.%s:\n"
               "\t.space 8\n", pool.idens.val[lexr.tok.val]);
        lexr_next();
        if (lexr.tok.cat == TOK_COMMA) {
            lexr_next();
            goto add_var;
        }
        if (lexr.tok.cat != TOK_SEMI) {
            err();
        }
        lexr_next();
    }
    output(".text\n");
    while (lexr.tok.cat == TOK_KW_PROC) {
        lexr_next();
        output("PROC.%s:\n", pool.idens.val[lexr.tok.val]);
        lexr_next();/*ident*/
        lexr_next(); /*;*/
        pars_blok();
        lexr_next(); /*;*/
        output("\tret\n");
    }
    output("main:\n");
    pars_stmt();
    output("\tcall pl.exit\n");
}

int main() {
    lexr_kickstart();
    pars_code();
    return 0;
}
