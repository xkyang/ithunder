#ifndef __EXPR__H
#define __EXPR__H
#include "ibase.h"
#define EXPR_ELEM_MAX  16
#pragma pack(push, 4)
typedef struct _ELEM
{
    int field; /* 字段 */
    int opnd;  /* 操作数 */
    int optr;  /* 操作符 */
}ELEM;
typedef struct _PAIR
{
    int from;
    int to;
}PAIR;

typedef struct _EXPR
{
    int num;
	int condition;
    PAIR int_range;
    PAIR long_range;
    PAIR double_range;
    ELEM list[EXPR_ELEM_MAX];
	void* logger;
}EXPR;
#define EXPXK(x) ((EXPR *)x)
EXPR *expr_init();
int expr_set(EXPR *expr, const char* str, int len);
int64_t expr_cal(EXPR *expr,IBSTATE* state,int secid,int docid);
void expr_int_range(EXPR *expr,int from,int to);
void expr_long_range(EXPR *expr,int from,int to);
void expr_double_range(EXPR *expr,int from,int to);
void expr_print_range(EXPR *expr);
void expr_set_logger(EXPR *expr,void* logger);
void expr_print(EXPR *expr);
void expr_clean(EXPR *expr);
void expr_close(EXPR *expr);
#pragma pack(pop)
#endif
