/*
** node.h - nodes of abstract syntax tree
**
** See Copyright Notice in mruby.h
*/

enum node_type {
    NODE_METHOD,
    NODE_FBODY,
    NODE_CFUNC,
    NODE_SCOPE,
    NODE_BLOCK,
    NODE_IF,
    NODE_CASE,
    NODE_WHEN,
    NODE_OPT_N,
    NODE_WHILE,
    NODE_UNTIL,
    NODE_ITER,
    NODE_FOR,
    NODE_BREAK,
    NODE_NEXT,
    NODE_REDO,
    NODE_RETRY,
    NODE_BEGIN,
    NODE_RESCUE,
    NODE_ENSURE,
    NODE_AND,
    NODE_OR,
    NODE_NOT,
    NODE_MASGN,
    NODE_ASGN,
    NODE_CDECL,
    NODE_CVASGN,
    NODE_CVDECL,
    NODE_OP_ASGN,
    NODE_CALL,
    NODE_FCALL,
    NODE_VCALL,
    NODE_SUPER,
    NODE_ZSUPER,
    NODE_ARRAY,
    NODE_ZARRAY,
    NODE_HASH,
    NODE_RETURN,
    NODE_YIELD,
    NODE_LVAR,
    NODE_DVAR,
    NODE_GVAR,
    NODE_IVAR,
    NODE_CONST,
    NODE_CVAR,
    NODE_NTH_REF,
    NODE_BACK_REF,
    NODE_MATCH,
    NODE_MATCH2,
    NODE_MATCH3,
    NODE_INT,
    NODE_FLOAT,
    NODE_NEGATE,
    NODE_LAMBDA,
    NODE_SYM,
    NODE_STR,
    NODE_DSTR,
    NODE_DREGX,
    NODE_DREGX_ONCE,
    NODE_LIST,
    NODE_ARG,
    NODE_ARGSCAT,
    NODE_ARGSPUSH,
    NODE_SPLAT,
    NODE_TO_ARY,
    NODE_SVALUE,
    NODE_BLOCK_ARG,
    NODE_DEF,
    NODE_SDEF,
    NODE_ALIAS,
    NODE_UNDEF,
    NODE_CLASS,
    NODE_MODULE,
    NODE_SCLASS,
    NODE_COLON2,
    NODE_COLON3,
    NODE_CREF,
    NODE_DOT2,
    NODE_DOT3,
    NODE_FLIP2,
    NODE_FLIP3,
    NODE_ATTRSET,
    NODE_SELF,
    NODE_NIL,
    NODE_TRUE,
    NODE_FALSE,
    NODE_DEFINED,
    NODE_NEWLINE,
    NODE_POSTEXE,
    NODE_ALLOCA,
    NODE_DMETHOD,
    NODE_BMETHOD,
    NODE_MEMO,
    NODE_IFUNC,
    NODE_DSYM,
    NODE_ATTRASGN,
    NODE_LAST
};
