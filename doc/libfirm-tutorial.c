#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libfirm/firm.h>

#define BUFFER_SIZE 512

// the lexer's tokens
typedef enum token_kind {
    TOK_EOF,       // end of file
    TOK_DEF,       // 'def'
    TOK_EXT,       // 'extern'
    TOK_ID,        // identifier
    TOK_NUM,       // number
    TOK_PLUS,      // '+'
    TOK_MINUS,     // '-'
    TOK_STAR,      // '*'
    TOK_LESS,      // '<'
    TOK_LBRACE,    // '('
    TOK_RBRACE,    // ')'
    TOK_COMMA,     // ','
} token_kind;

// needed to identify the expressions in expr_t
typedef enum expr_kind {
    EXPR_NUM,               // number expression
    EXPR_VAR,               // variable expression
    EXPR_BIN,               // binary expression
    EXPR_CALL,              // call expression
} expr_kind;

// container struct for all kinds of expressions
typedef struct expr_t expr_t; // forward declaration
struct expr_t {
    void *expr;             // the actual expression
    expr_kind which;        // its kind

    expr_t *next;           // pointer to the next expression
};

typedef struct num_expr_t {
    int val;             // the value of the number
} num_expr_t;

typedef struct var_expr_t {
    char *name;             // the name of a variable
} var_expr_t;

typedef struct bin_expr_t {
    char op;                // the binary operator
    expr_t *lhs, *rhs;      // the left and right hand side
} bin_expr_t;

typedef struct call_expr_t {
    char *callee;           // the function to be called
    expr_t *args;           // the arguments of the call
    int argc;               // the number of arguments
} call_expr_t;

typedef struct parameter_t parameter_t; // forward declaration
struct parameter_t {
    char *name;             // the parameter name
    ir_node *proj;

    parameter_t *next;
};

typedef struct prototype_t prototype_t; // forward declaration
struct prototype_t {
    char *name;
    parameter_t *args;      // the argument names
    int argc;               // the number of arguments
    ir_entity *ent;         // the entity of the function represented by this prototype

    prototype_t *next;      // pointer to the next prototype
};

typedef struct function_t function_t; // forward declaration
struct function_t {
    prototype_t *head;      // the head of the function
    expr_t *body;           // the function body, i.e. a expression

    function_t *next;       // pointer to the next function
};

// global variables needed by the lexer
static FILE *input;

// global variables needed by the parser
static int cur_token;               // our current token
static char *id_str;                // the identifier string
static int num_val;                 // contains the value of any numbers

// the AST
static expr_t *main_exprs;          // a list containing the top level expressions
static expr_t *last_main_expr;      // the tail of that list
static prototype_t *prototypes;     // a list containing the prototypes
static function_t *functions;       // a list containing the functions
static ir_mode *int_mode;           // mode used for int
static ir_type *int_type;           // type used for int

// returns the current token
static token_kind next_token(void) {
    static int ch = ' ';                            // the current character
    char buffer[BUFFER_SIZE];                       // the input buffer

    // skip whitespace
    while (isspace(ch))
        ch = fgetc(input);

    // keyword or identifier
    switch (ch) {
        case '+':
            ch = fgetc(input);
            return TOK_PLUS;

        case '-':
            ch = fgetc(input);
            return TOK_MINUS;

        case '*':
            ch = fgetc(input);
            return TOK_STAR;

        case '<':
            ch = fgetc(input);
            return TOK_LESS;

        case '(':
            ch = fgetc(input);
            return TOK_LBRACE;

        case ')':
            ch = fgetc(input);
            return TOK_RBRACE;

        case ',':
            ch = fgetc(input);
            return TOK_COMMA;

        case EOF:
            return TOK_EOF; // end of file

        case '#':

            // after the comment character we can ignore the rest of the line
            do {
                ch = fgetc(input);
            }
            while (ch != EOF && ch != '\n' && ch != '\r');

            // return what comes after the comment
            return next_token();

        default:
            if (isalpha(ch)) {
                // read in all successive alphanumeric characters
                int i = 0;

                do {
                    buffer[i++] = ch;
                    ch = fgetc(input);
                }
                while (isalnum(ch));

                buffer[i] = '\0';

                // check for the keywords
                if (strcmp(buffer, "def") == 0)
                    return TOK_DEF;
                else if (strcmp(buffer, "extern") == 0)
                    return TOK_EXT;
                else {
                    // it is an identifier
                    id_str = calloc(strlen(buffer) + 1, sizeof(char));
                    strcpy(id_str, buffer);
                    return TOK_ID;
                }
            }
            else if (isdigit(ch)) {
                // read in all successive digits
                int i = 0;

                do {
                    buffer[i++] = ch;
                    ch = fgetc(input);
                }
                while (isdigit(ch));

                buffer[i] = '\0';

                num_val = atoi(buffer);              // convert to int number
                return TOK_NUM;
            }
            else {
                fprintf(stderr, "Error: Unexpected character '%c'\n", ch);
                ch = fgetc(input);
                return next_token();
            }
    }
}

// constructors for the various expression structures
static expr_t *new_expr(void *expr, expr_kind which) {
    expr_t *e = calloc(1, sizeof(expr_t));
    e->expr = expr;
    e->which = which;
    return e;
}

static num_expr_t *new_num_expr(int val) {
    num_expr_t *n = calloc(1, sizeof(num_expr_t));
    n->val = val;
    return n;
}

static var_expr_t *new_var_expr(char *name) {
    var_expr_t *v = calloc(1, sizeof(var_expr_t));
    v->name = name;
    return v;
}

static bin_expr_t *new_bin_expr(char op, expr_t *lhs, expr_t *rhs) {
    bin_expr_t *b = calloc(1, sizeof(bin_expr_t));
    b->op = op;
    b->lhs = lhs;
    b->rhs = rhs;
    return b;
}

static call_expr_t *new_call_expr(char *callee, expr_t *args, int argc) {
    call_expr_t *c = calloc(1, sizeof(call_expr_t));
    c->callee = callee;
    c->args = args;
    c->argc = argc;
    return c;
}

static parameter_t *new_parameter(char *name) {
    parameter_t *p = calloc(1, sizeof(parameter_t));
    p->name = name;
    return p;
}

static prototype_t *new_prototype(char *name, parameter_t *args, int argc) {
    prototype_t *p = calloc(1, sizeof(prototype_t));
    p->name = name;
    p->args = args;
    p->argc = argc;
    return p;
}

static function_t *new_function(prototype_t *head, expr_t *body) {
    function_t *f = calloc(1, sizeof(function_t));
    f->head = head;
    f->body = body;
    return f;
}

// Parser interface
static expr_t *parse_expr(void);

// error printing
static void error(char *msg, char *info) {
    fprintf(stderr, "Error: %s%s.\n", msg, info);
}

// check if a prototype exists for the called function
// and if the number of parameters is correct
static bool check_call(call_expr_t *call) {
    for (prototype_t *p = prototypes; p != NULL; p = p->next) {
        if (!strcmp(call->callee, p->name) && call->argc == p->argc)
            return true;
    }

    return false;
}

// parses an identifier expression
static expr_t *parse_id_expr(void) {
    char *identifier = id_str;
    expr_t *id_expr = NULL;

    cur_token = next_token();

    if (cur_token != TOK_LBRACE) {                                  // it's not a call
        id_expr = new_expr(new_var_expr(identifier), EXPR_VAR);     // hence it's a variable
    }
    else {
        expr_t *args = NULL;
        int c = 0;                                                  // the number of arguments
        cur_token = next_token();

        if (cur_token != TOK_RBRACE) {                              // the call has arguments
            while (1) {
                expr_t *e = parse_expr();                           // an argument is an expression

                if (e == NULL) {                                    // error checking
                    args = NULL;
                    break;
                }

                e->next = args;                                     // link the arguments
                args = e;
                c++;                                                // and count them

                if (cur_token == TOK_RBRACE)                        // we're done
                    break;

                if (cur_token != TOK_COMMA) {                       // error checking
                    error("Expected ')' or ',' in argument list", "");
                    break;
                }

                cur_token = next_token();
            }
        }

        cur_token = next_token();
        // create the call expression
        call_expr_t *call = new_call_expr(identifier, args, c);

        if (check_call(call)) {                                     // check if the call is valid
            id_expr = new_expr(call, EXPR_CALL);                    // wrap it up
        }
    }

    return id_expr;
}

// parse a number expression
static expr_t *parse_num_expr(void) {
    num_expr_t *expr = new_num_expr(num_val);
    cur_token = next_token();

    return new_expr(expr, EXPR_NUM);
}

// parse a parenthesis expression
static expr_t *parse_paren_expr(void) {
    cur_token = next_token();                                                   // eat the '('
    expr_t *result = parse_expr();                                  // parse the expression

    if (cur_token != TOK_RBRACE) {                                  // error checking
        error("')' expected", "");
        result = NULL;
    }

    return result;
}

// parse a primary expression
static expr_t *parse_primary(void) {
    switch (cur_token) {
        case TOK_ID:
            return parse_id_expr();

        case TOK_NUM:
            return parse_num_expr();

        case TOK_LBRACE:
            return parse_paren_expr();

        default: {
            char info[2] = { cur_token, '\0' };
            error("Unknown token when expecting an expression: ", info);
            return NULL;
        }
    }
}

// returns the precedence of the current token
static int get_tok_prec(void) {
    switch (cur_token) {
        default:
            return -1;

        case TOK_LESS:
            return 10;

        case TOK_PLUS:
            return 20;

        case TOK_MINUS:
            return 20;

        case TOK_STAR:
            return 40;
    }
}

// parse the right hand side of a binary expression
static expr_t *parse_bin_rhs(int min_prec, expr_t *lhs) {
    while (true) {
        // get the precedence of the current token
        int tok_prec = get_tok_prec();

        // if it is lower than the minimum precedence, return the lhs
        if (tok_prec < min_prec)
            return lhs;

        // our binary operator is the current token
        int bin_op = cur_token;
        cur_token = next_token();
        // parse the rhs as primary expression
        expr_t *rhs = parse_primary();

        if (!rhs)
            return NULL;

        // get the precedence of the current token
        int next_prec = get_tok_prec();

        // if it is higher than the precedence of our binop
        // then our rhs is the lhs of a binary expression with higher precedence
        if (tok_prec < next_prec) {
            // parse the pending binary operation first, with our rhs as its lhs
            rhs = parse_bin_rhs(tok_prec + 1, rhs);

            if (!rhs)
                return NULL;
        }

        // merge lhs and rhs
        lhs = new_expr(new_bin_expr(bin_op, lhs, rhs), EXPR_BIN);
    }
}

// parse a prototype
static prototype_t *parse_prototype(void) {
    parameter_t *args = NULL;
    int argc = 0;                                                   // number of arguments
    char *fn_name = NULL;                                           // function name

    if (cur_token != TOK_ID)
        error("Expected function name in prototype", "");
    else {
        fn_name = id_str;
        cur_token = next_token();                                   // eat the name

        if (cur_token != TOK_LBRACE)
            error("Expected '(' in prototype", "");
        else {
            while ((cur_token = next_token()) == TOK_ID) {          // get the arguments
                parameter_t *arg = new_parameter(id_str);
                arg->next = args;
                args = arg;
                argc++;
            }

            if (cur_token != TOK_RBRACE) {
                error("Expected ')' in prototype", "");
                args = NULL;
            }
        }
    }

    cur_token = next_token();

    // if a prototype with this name already exists, don't create another one
    for (prototype_t *p = prototypes; p != NULL; p = p->next) {
        if (!strcmp(p->name, fn_name))
            return p;
    }

    // the prototype doesn't already exist
    prototype_t *proto = new_prototype(fn_name, args, argc);
    proto->next = prototypes;
    prototypes  = proto;
    return proto;
}

// parse the definition of a function
static bool parse_definition(void) {
    cur_token = next_token();

    prototype_t *head = parse_prototype();                     // its head is a prototype
    expr_t *body = parse_expr();                               // its body is an expression

    // check the results
    if (head != NULL && body != NULL) {
        function_t *fn = new_function(head, body);             // create the function struct
        fn->next = functions;                                  // put it in the global function list
        functions = fn;
        return true;
    }
    else
        return false;
}

// parse an expression
static expr_t *parse_expr(void) {
    expr_t *lhs = parse_primary();

    if (!lhs)
        return NULL;

    return parse_bin_rhs(0, lhs);
}

// parse a top level expression
static bool parse_top_lvl(void) {
    expr_t *expr = parse_expr();                                // parse it like an ordinary expression

    if (expr == NULL)                                           // check the result
        return false;                                           // failure

    // add it to the end of the main function expression list
    if (last_main_expr != NULL)
        last_main_expr->next = expr;
    else
        main_exprs = expr;

    last_main_expr = expr;

    return true;                                                // success
}

// the main parser function
static bool parse(void) {
    cur_token = next_token();

    // parse the file till we reach the end
    while (true) {
        switch (cur_token) {
            case TOK_EOF:
                return true;

            case TOK_DEF:
                if (!parse_definition())
                    return false;

                break;

            case TOK_EXT:
                cur_token = next_token();

                if (parse_prototype() == NULL)
                    return false;

                break;

            case ';':
                cur_token = next_token();
                break;

            default:
                if (!parse_top_lvl())
                    return false;

                break;
        }
    }
}

// creates an entity for each prototype
static void create_prototype_entities(void) {
    for (prototype_t *proto = prototypes; proto != NULL; proto = proto->next) {
        ir_type *proto_type = new_type_method(proto->argc, 1);  // create a method type

        for (int i = 0; i < proto->argc; i++)                   // for each parameter
            set_method_param_type(proto_type, i, int_type);     // set its type to double

        set_method_res_type(proto_type, 0, int_type);           // do the same for the result type
        // that's all we need to create the entity
        proto->ent = new_entity(get_glob_type(), new_id_from_str(proto->name), proto_type);
    }
}

static ir_node *handle_expr(expr_t *expr, parameter_t *args); // forward declaration


static ir_node *handle_num_expr(num_expr_t *num_expr, parameter_t *args) {
    (void)args;
    ir_tarval *tv = new_tarval_from_long(num_expr->val, mode_Is);
    return new_Const(tv);
}

// generates the nodes for a binary expression
static ir_node *handle_bin_expr(bin_expr_t *bin_ex, parameter_t *args) {
    // lhs and rhs are both expressions
    ir_node *lhs = handle_expr(bin_ex->lhs, args);
    ir_node *rhs = handle_expr(bin_ex->rhs, args);

    switch (bin_ex->op) {
        case TOK_LESS:
            return new_Cmp(lhs, rhs, ir_relation_less); // compare lhs with rhs

        case TOK_PLUS:
            return new_Add(lhs, rhs, int_mode);

        case TOK_MINUS:
            return new_Sub(lhs, rhs, int_mode);

        case TOK_STAR:
            return new_Mul(lhs, rhs, int_mode);

        default:
            error("Invalid binary expression", "");
            return NULL;
    }
}

// returns the proj for the given parameter
static ir_node *handle_var_expr(var_expr_t *var, parameter_t *args) {
    for (parameter_t *p = args; p != NULL; p = p->next) {
        if (!strcmp(p->name, var->name))
            return p->proj;
    }

    return NULL;
}

// generates the nodes for the given call expression
static ir_node *handle_call_expr(call_expr_t *call, parameter_t *args) {
    // find the corresponding prototype
    ir_entity *ent;

    for (prototype_t *p = prototypes;; p = p->next) {
        if (p == NULL) {
            error("Cannot call unknown function: ", call->callee);
            return NULL;
        }

        if (!strcmp(p->name, call->callee)) {
            ent = p->ent;
            break;
        }
    }

    // allocate space for an array referencing the arguments
    ir_node **in = calloc(call->argc, sizeof(*in));
    // handle the arguments
    int i = 0;

    for (expr_t *e = call->args; e != NULL; e = e->next)
        in[i++] = handle_expr(e, args);

    // create the call
    ir_node *store     = get_store();
    ir_node *callee    = new_Address(ent);
    ir_node *call_node = new_Call(store, callee, call->argc, in, get_entity_type(ent));
    // update the current store
    ir_node *new_store = new_Proj(call_node, get_modeM(), pn_Call_M);
    set_store(new_store);
    // get the result
    ir_node *tuple  = new_Proj(call_node, get_modeT(), pn_Call_T_result);
    ir_node *result = new_Proj(tuple, int_mode, 0);

    free(in);
    return result;
}

// decides what to do with the given expression
static ir_node *handle_expr(expr_t *expr, parameter_t *args) {
    switch (expr->which) {
        case EXPR_NUM:
            return handle_num_expr((num_expr_t *) expr->expr, args);

        case EXPR_BIN:
            return handle_bin_expr((bin_expr_t *) expr->expr, args);

        case EXPR_VAR:
            return handle_var_expr((var_expr_t *) expr->expr, args);

        case EXPR_CALL:
            return handle_call_expr((call_expr_t *) expr->expr, args);

        default:
            abort();
    }
}

// creates an ir_graph for each function
static void create_func_graphs(void) {
    for (function_t *fn = functions; fn != NULL; fn = fn->next) {
        int n_param = fn->head->argc;
        ir_graph *fun_graph = new_ir_graph(fn->head->ent, n_param); // create a new graph
        set_current_ir_graph(fun_graph);

        // create the projs for the parameters
        if (n_param > 0) {
            // keep track of the current block
            ir_node *block = get_r_cur_block(fun_graph);
            // set the start block to be the current block
            set_r_cur_block(fun_graph, get_irg_start_block(fun_graph));
            // get a reference to the arguments node
            ir_node *args = get_irg_args(fun_graph);

            int i = 0;                                              // for each argument

            for (parameter_t *p = fn->head->args; p != NULL; p = p->next) {
                p->proj = new_Proj(args, int_mode, i++);      // create a projection node
            }

            set_r_cur_block(fun_graph, block);                // restore the current block
        }

        // the body is just an expression
        ir_node *node = handle_expr(fn->body, fn->head->args);
        // the result of the function is the result of the body
        ir_node *results[1] = { node };
        ir_node *store = get_store();
        ir_node *ret = new_Return(store, 1, results);               // create a return node
        ir_node *end = get_irg_end_block(fun_graph);                // get hold of the end block
        // set the return node to be its predecessor
        add_immBlock_pred(end, ret);
        mature_immBlock(get_r_cur_block(fun_graph));          // mature the current block

        irg_finalize_cons(fun_graph);                               // finalize the construction
    }
}

// create the main graph
static void create_main(void) {
    ir_type *type = new_type_method(0, 1);                // create the type
    set_method_res_type(type, 0, int_type);               // set the result type
    // create an entity
    ir_entity *ent = new_entity(get_glob_type(), new_id_from_str("__simple_main"), type);
    // create a fresh graph
    ir_graph *fn_main = new_ir_graph(ent, 0);
    set_current_ir_graph(fn_main);

    // handle each expression and keep a reference to the last one
    ir_node *node = NULL;

    for (expr_t *e = main_exprs; e != NULL; e = e->next)
        node = handle_expr(e, NULL);

    /* just use a 0 constant if there were no expressions */
    if (node == NULL)
        node = new_Const(get_mode_null(int_mode));

    // get the result
    ir_node *store = get_store();
    ir_node *results[1] = { node };
    ir_node *ret = new_Return(store, 1, results);
    add_immBlock_pred(get_irg_end_block(fn_main), ret);
    // mature the current block
    mature_immBlock(get_r_cur_block(fn_main));
    // finalize the construction
    irg_finalize_cons(fn_main);
}

int main(int argc, char **argv) {
    char *src_name = NULL;
    bool opt_dump = false;                      // '-d' as the first argument enables dumping

    // handle the arguments
    if (argc == 2)
        src_name = argv[1];
    else if (argc == 3 && strcmp(argv[1], "-d") == 0) {
        src_name = argv[2];
        opt_dump = true;
    }
    else {
        error("No source file provided", "");
        exit(1);
    }

    // open the source file
    input = fopen(src_name, "r");

    if (input == NULL) {
        error("Could not open source file: ", strerror(errno));
        exit(1);
    }

    bool parse_fine = parse();

    if (!parse_fine) {
        error("Had a parse error, aborting", "");
        exit(1);
    }

    ir_init();                              // initialize libFirm

    int_mode = mode_Is;
    int_type = new_type_primitive(int_mode);
    // create the graphs
    create_prototype_entities();
    create_func_graphs();
    create_main();

    if (opt_dump)                           // dump all graphs
        dump_all_ir_graphs("");

    be_main(stdout, src_name);              // emit code
    ir_finish();                            // clean up

    return 0;
}
