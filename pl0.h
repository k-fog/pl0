#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// tokenize
typedef enum {
    TK_IDENT,
    TK_OP,
    TK_PUNCT,
    TK_NUM,
    TK_EOF,
} TokenType;

typedef struct Token Token;
struct Token {
    TokenType type;
    char *str;
    int len;
    int val;
    Token *next;
};

extern Token *current_token;

Token *new_token(TokenType type, Token *cur, char *str, int len);
Token *tokenize(char *src);
char *to_string(char *s, int len);
void view_tokens(Token *tok);
Token *peek();
Token *read();


// parse
typedef enum {
    ND_IDENT,     // identifier
    ND_NUM,       // number
    ND_ADD,       // +
    ND_SUB,       // -
    ND_MUL,       // *
    ND_DIV,       // /
    ND_MOD,       // %
    ND_EQ,	      // =
    ND_LT,        // <
    ND_LE,        // <=
    ND_NE,        // <>
    ND_ODD,       // odd
    ND_IF,        // if statement
    ND_WHILE,     // while statement
    ND_RET,       // return
    ND_WRITE,     // write
    ND_WRITELN,   // writeln
    ND_ASSG,      // assignment
    ND_BEGIN,     // begin ... end
    ND_VARS,      // var
    ND_CONST,     // const
    ND_CONSTDEF,  // const
    ND_FNDEF,     // function
    ND_ARGS,      // args
    ND_PARAMS,    // parameters
    ND_FNCALL,    // function call 
    ND_BLOCK,     // block
    ND_NULL,      // null
} NodeType;

typedef struct Node Node;
struct Node {
    NodeType type;
    Node *lhs;
    Node *rhs;

    Node *condition;
    Node *body;

    Node *next;

    Node *name;
    Node *args;
    Node *params;

    int val;
    char *str;
};

Node *parse(Token *tok);
void view_ast(Node *node);


// data type & hashmap

typedef enum {
    P_INT,
    P_FUNC,
    P_RETINT,
} pType;

typedef struct pVal {
    pType type;
    union {
        int intnum;
        Node *func;
    } val;
    bool is_const;
} pVal;

pVal *pInt(int x);
pVal *pFunc(Node *func);

typedef struct {
    char *key;
    pVal *val;
} Pair;

typedef struct {
    int size;
    Pair **data;
} Hashmap;

Hashmap *new_hashmap(int size);
Pair *new_pair(char *key, pVal *val);
void add2map(Hashmap *map, Pair *pair);
Pair *get_from_map(Hashmap *map, char *key);
void update_map(Hashmap *map, Pair *new_pair);
bool haskey(Hashmap *map, char *key);


// eval
#define HASHMAX 23
typedef struct _Env Env;
struct _Env {
    Env *outer;
    Hashmap *var;
};

Env *new_env(Env *outer);
pVal *eval(Node *node, Env *env);

extern Env *global_env;
