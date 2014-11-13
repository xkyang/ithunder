#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "expr.h"
#include "imap.h"
#include "lmap.h"
#include "dmap.h"
#include "logger.h"

//#defind EXPR_DEBUG 1
#define TIME_REFRESH_SHIFT 5
#define ISNUM(p) ((*p >= '0' && *p <= '9'))
#define ISSIGN(p) ((*p == ' ' || *p == '\t' || *p == '\n'))
#define ISOPND(p) ((*p == '-') || (*p >= '0' && *p <= '9'))
#define ISOPTR(p) ((*p == '+' || *p == '*' || *p == '/' || *p == '%' || *p == '<' || *p == '>' || *p == '@' || *p == '=' || *p == '!'))
EXPR *expr_init()
{
    EXPR *expr = NULL;
    if((expr = (EXPR *)calloc(1, sizeof(EXPR))) != NULL)
    {
       memset(expr,0,sizeof(EXPR));
    }
    return expr;
}

void expr_set_logger(EXPR *expr,void* logger)
{
    if(expr)
	{
	   expr->logger = logger;
	}
}

void expr_int_range(EXPR *expr,int from,int to)
{
   if(expr)
   {
      expr->int_range.from = from;
      expr->int_range.to = to;
   }
}

void expr_long_range(EXPR *expr,int from,int to)
{
   if(expr)
   {
      expr->long_range.from = from;
      expr->long_range.to = to;
   }
}

void expr_double_range(EXPR *expr,int from,int to)
{
   if(expr)
   {
      expr->double_range.from = from;
      expr->double_range.to = to;
   }
}

void expr_print_range(EXPR *expr)
{
   if(expr)
   {
      ACCESS_LOGGER(expr->logger,"int_range:(%d -> %d)\n",expr->int_range.from,expr->int_range.to);
      ACCESS_LOGGER(expr->logger,"long_range:(%d -> %d)\n",expr->long_range.from,expr->long_range.to);
      ACCESS_LOGGER(expr->logger,"double_range:(%d -> %d)\n",expr->double_range.from,expr->double_range.to);
   }
}

void expr_clean(EXPR *expr)
{
   if(expr)
   {
      expr->num = 0;	   
      memset(expr->list,0,sizeof(ELEM)*EXPR_ELEM_MAX);
   }
}

void expr_print(EXPR *expr)
{
   if(expr)
   {
      int i = 0;
      for(;i<expr->num;++i)
      {
         ACCESS_LOGGER(expr->logger,"i:%d field:%d opnd:%d optr:%c\n",i,expr->list[i].field,
			expr->list[i].opnd,expr->list[i].optr);
      }
   }
}

int expr__field__valid(EXPR *expr,int field)
{
   if(expr)
   {
       if((field >= expr->int_range.from) && (field < expr->int_range.to))
       {
          return 1;
       } 
       else if((field >= expr->long_range.from) && (field < expr->long_range.to))
       {
          return 1;
       } 
       else if((field >= expr->double_range.from) && (field < expr->double_range.to))
       {
          return 1;
       } 
       else
       {
          return 0;
       }
   }
   return 0;
}

/* expr set val */
int expr_set(EXPR *expr, const char* str, int len)
{
    int ret = -1, k = -1, field = 0, opnd = 0, optr = 0;
    if(expr && str && (len > 0))
    {
        ACCESS_LOGGER(expr->logger, "DEBUG:[%s] for expr_set\n", str);
        const char* p = str;
	    while(*p!='\0')
	    {
	       field = 0;
	       opnd = 0;
	       optr = 0;
	       while((*p!='\0')&&(!ISNUM(p)))++p;
           if(*p!='\0')
	       {
	          field = atoi(p);
	          while((*p!='\0')&&(ISNUM(p)))++p;
	          while((*p!='\0')&&(!ISOPTR(p)))++p;
              if(*p!='\0')
	          {
                 optr = *p++; 
		         if(optr != '+')
		         {
                    if((optr == '<') ||(optr == '>'))
                    {
                       if((*p!='\0')&&(*p == optr))
                       {
			             if((field >= expr->double_range.from) && 
			                (field < expr->double_range.to))
			             {
                            ACCESS_LOGGER(expr->logger, "ERROR:[%s] for double[%d:%d] shift\n", str,
		                    expr->double_range.from,expr->double_range.to);
                             return -1;
			             }
                         ++p;
                       }
                       else //wrong expression
                       {
                          ACCESS_LOGGER(expr->logger, "ERROR:[%s] for shift\n", str);
                          return -1;
                       }
                    }
                    while((*p!='\0')&&(!ISOPND(p)))++p;
                    if(*p!='\0')
                    {
                       opnd = atoi(p);     
                       while((*p!='\0')&&(ISOPND(p)))++p;
                    }
		            if(((optr == '/') || (optr == '%')) && (opnd == 0))
		            {
                       ACCESS_LOGGER(expr->logger, "ERROR:[%s] for divide 0\n", str);
		               return -1;
		            }
	                while((*p!='\0')&&(!ISOPTR(p)))++p;
                    if(*p!='\0')
	                {
                       if(*p != '+') //wrong expression
	                   {
                          ACCESS_LOGGER(expr->logger, "ERROR:[%s] for concat(+):%c\n", str,*p);
	                      return -1;
	                   }
	                   else
	                   {
	                     ++p;
	                   }
	                }
		         }
            }
	     }
	     if(expr__field__valid(expr,field) == 0)
	     {
           ACCESS_LOGGER(expr->logger, "ERROR:[%d] is wrong range\n", field);
	       return -1;
	     }
	     if((optr == '@') && (expr__field__valid(expr,opnd) == 0)) //时间刷新
	     {
           ACCESS_LOGGER(expr->logger, "ERROR:time refresh [%d] is wrong range\n", opnd);
	       return -1;
	     }
	     if(expr->num < EXPR_ELEM_MAX)
	     {
	       k = expr->num++;	   
	       expr->list[k].field = field;
	       expr->list[k].opnd = opnd;
	       expr->list[k].optr = optr;
	     }
	     else
	     {
           ACCESS_LOGGER(expr->logger, "WARNING:expr num is much more field:%d\n", field);
	       break;
	    }
	  }
      ret = k;
    }
    return ret;
}

int64_t expr__time__refresh(EXPR* expr,long post_at,long refresh_at)
{
    long now = (long)time(NULL);
    struct tm tm_begin;
    localtime_r(&now, &tm_begin);
    tm_begin.tm_hour = 0;
    tm_begin.tm_min = 0;
    tm_begin.tm_sec = 0;
    long day_begin = (long)mktime(&tm_begin);
    long old_time = (long)(now - 3600 - ((float)(now - post_at) / 86400) * 1800);
    if(refresh_at > old_time)
    {
       old_time = refresh_at;
    }
    long new_time = (day_begin > old_time) ? day_begin : old_time;
	if(expr)
	{
       ACCESS_LOGGER(expr->logger, "TIME:now:%ld post_at:%ld max(day_begin:%ld old_time:%ld refresh_at:%ld) = new_time:%ld\n", now,post_at,day_begin,old_time,refresh_at,new_time);
	}
    return new_time << TIME_REFRESH_SHIFT;
}

int64_t expr__map__value(EXPR* expr,IBSTATE* state,int field,int secid,int docid)
{
    int64_t value = 0;
    int fid = 0;;
    if(expr && state && (secid >= 0))
    {
       if((field >= expr->int_range.from) && (field < expr->int_range.to))
       {
	      fid = field - expr->int_range.from + IB_INT_OFF;
          value = IB_LONG_INT(IMAP_GET(state->mfields[secid][fid], docid));
       } 
       else if((field >= expr->long_range.from) && (field < expr->long_range.to))
       {
	      fid = field - expr->long_range.from + IB_LONG_OFF;
          value = IB_LONG_LONG(LMAP_GET(state->mfields[secid][fid], docid));
       } 
       else if((field >= expr->double_range.from) && (field < expr->double_range.to))
       {
	      fid = field - expr->double_range.from + IB_DOUBLE_OFF;
          value = IB_LONG_SCORE(DMAP_GET(state->mfields[secid][fid], docid));
       } 
	}
    ACCESS_LOGGER(expr->logger,"field:%d fid:%d value:%ld in secid:%d docid:%d\n",field,fid,value,secid,docid);
	return value;
}

int64_t expr__operate(EXPR* expr,IBSTATE* state,int i,int secid,int docid)
{
    int64_t ret = 0;
    int64_t value = 0, value2 = 0;
    int field = 0;
    ELEM* e = NULL;
    if(expr && state)
    {
       e = &(expr->list[i]);	    
       field = e->field;
	   value = expr__map__value(expr,state,field,secid,docid);
       switch(e->optr)
       {
          case '*':
			if(0 == expr->condition)
			{
   	           ret = value * e->opnd; 
			}
			else
			{
			   --(expr->condition);
			}
   	        break;
          case '/':
			if(0 == expr->condition)
			{
   	           ret = value / e->opnd; 
			}
			else
			{
			   --(expr->condition);
			}
   	        break;
          case '%':
			if(0 == expr->condition)
			{
   	           ret = value % e->opnd; 
			}
			else
			{
			   --(expr->condition);
			}
   	        break;
          case '<': //左移
			if(0 == expr->condition)
			{
   	           ret = value << e->opnd; 
			}
			else
			{
			   --(expr->condition);
			}
   	        break;
          case '>': //右移
			if(0 == expr->condition)
			{
   	           ret = value >> e->opnd; 
			}
			else
			{
			   --(expr->condition);
			}
   	        break;
          case '@': //时间刷新
			if(0 == expr->condition)
			{
   	           ret = expr__map__value(expr,state,e->opnd,secid,docid);
			}
			else
			{
	           value2 = expr__map__value(expr,state,e->opnd,secid,docid);
   	           ret = expr__time__refresh(expr,value,value2); 
			   --(expr->condition);
			}
   	        break;
          default:  //加法
   	        ret = value; 
   	   break;
       }
       ACCESS_LOGGER(expr->logger,"i:%d field:%d opnd:%d optr:%c value/2:%ld/%ld result:%ld in secid:%d docid:%d\n",i,field,e->opnd,e->optr,value,value2,ret,secid,docid);
    }
    return ret;
}

int expr__condition(EXPR* expr,IBSTATE* state,int i,int secid,int docid)
{
	int ret = 0;
    int64_t value = 0;
    int field = 0;
    ELEM* e = NULL;
	if(expr && state)
	{
       e = &(expr->list[i]);	    
       switch(e->optr)
       {
          case '=':
            field = e->field;
	        value = expr__map__value(expr,state,field,secid,docid);
  		    expr->condition = (value == e->opnd) ? 0 : 1;
			ret = 1;
  	        break;
          case '!':
            field = e->field;
	        value = expr__map__value(expr,state,field,secid,docid);
  		    expr->condition = (value != e->opnd) ? 0 : 1;
			ret = 1;
  	        break;
         default:
			expr->condition = (expr->condition > 0) ? expr->condition : 0;
  	        break;
		 /*	
         case '?':
  	        expr->condition = (value == e->opnd) ? 1 : 2; 
			ret = 1;
  	        break;
         default:
			expr->condition = (expr->condition > 0) ? --(expr->condition) : 0;
  	        break;
		 */	
      }
	}
	return ret;
}

/* expr cal val */
int64_t expr_cal(EXPR *expr,IBSTATE* state,int secid,int docid)
{
    int64_t ret = 0;
    if(expr && state)
    {
        int i = 0;
        for(;i<expr->num;++i)
        {
		    if(expr__condition(expr,state,i,secid,docid) != 0)
		    {
		       continue;
		    }
 	        ret += expr__operate(expr,state,i,secid,docid);
        }
    }
    ACCESS_LOGGER(expr->logger, "expr_cal result:%ld\n", ret);
    return ret;
}

void expr_close(EXPR *expr)
{
    if(expr)
    {
       free(expr);
    }
    return ;
}

int expr__test__condition(EXPR* expr,ELEM* e)
{
	int ret = 0;
	if(expr)
	{
       switch(e->optr)
       {
          case '=':
  		    expr->condition = (e->field == e->opnd) ? 0 : 1;
			ret = 1;
  	        break;
          case '!':
  		    expr->condition = (e->field != e->opnd) ? 0 : 1;
			ret = 1;
  	        break;
         default:
			expr->condition = (expr->condition > 0) ? expr->condition : 0;
  	        break;
		 /*	
         case '?':
  	        expr->condition = (e->field == e->opnd) ? 1 : 2; 
			ret = 1;
  	        break;
         default:
			expr->condition = (expr->condition > 0) ? --(expr->condition) : 0;
  	        break;
		 */	
      }
	}
	return ret;
}

int64_t expr__test__operate(EXPR* expr,ELEM* e)
{
    int64_t ret = 0;
	if(expr)
	{
        switch(e->optr)
        {
           case '*':
		     if(0 == expr->condition)
		     {
    	        ret = ((int64_t)e->field) * e->opnd;
		     }
			 else
			 {
			    --(expr->condition);
			 }
    	     break;
           case '/':
		     if(0 == expr->condition)
		     {
    	        ret = ((int64_t)e->field) / e->opnd;
		     }
			 else
			 {
			    --(expr->condition);
			 }
    	     break;
           case '%':
		     if(0 == expr->condition)
		     {
    	        ret = ((int64_t)e->field) % e->opnd;
		     }
			 else
			 {
			    --(expr->condition);
			 }
    	     break;
           case '<': //左移
		     if(0 == expr->condition)
		     {
    	        ret = ((int64_t)e->field) << e->opnd;
		     }
			 else
			 {
			    --(expr->condition);
			 }
    	     break;
           case '>': //右移
		     if(0 == expr->condition)
		     {
    	        ret = ((int64_t)e->field) >> e->opnd;
		     }
			 else
			 {
			    --(expr->condition);
			 }
    	     break;
           case '@': //时间更新
		     if(0 == expr->condition)
		     {
    	        ret = (int64_t)e->opnd;
		     }
			 else
			 {
    	        ret = expr__time__refresh(expr,e->field, e->opnd); 
			    --(expr->condition);
			 }
    	     break;
           default:  //加法
    	     ret = (int64_t)e->field; 
    	     break;
        }
	}
    return ret;
}

/* expr cal val */
int64_t expr__test__cal(EXPR *expr)
{
    int64_t ret = 0;
    int64_t value = 0;
    if(expr)
    {
      int i = 0;
      for(;i<expr->num;++i)
      {
		 if(expr__test__condition(expr,&expr->list[i]) != 0)
		 {
		    continue;
		 }
	     value = expr__test__operate(expr,&(expr->list[i]));
	     ret += value;
#ifdef EXPR_DEBUG
         fprintf(stdout,"i:%d field:%d opnd:%d optr:%c value:%ld result:%ld\n",i,
			expr->list[i].field,expr->list[i].opnd,expr->list[i].optr,value,ret);
#endif	       
      }
    }
    return ret;
}

void expr__print__range(EXPR *expr)
{
   if(expr)
   {
      fprintf(stdout,"int_range:(%d -> %d)\n",expr->int_range.from,expr->int_range.to);
      fprintf(stdout,"long_range:(%d -> %d)\n",expr->long_range.from,expr->long_range.to);
      fprintf(stdout,"double_range:(%d -> %d)\n",expr->double_range.from,expr->double_range.to);
   }
}

#ifdef EXPR_TEST
#define BUFFERSIZE 256
//gcc -o expr expr.c -DEXPR_TEST -DEXPR_DEBUG && ./expr
int main()
{
    EXPR *expr = NULL;
    if((expr = expr_init()))
    {
/*	    
	  expr_int_range(expr,3,11);
	  expr_long_range(expr,11,20);
	  expr_double_range(expr,20,25);
*/	
	  expr_int_range(expr,1,2147483647);
	  expr_long_range(expr,1,2147483647);
	  expr_double_range(expr,1,7);
	  expr__print__range(expr);
      fprintf(stdout, "sizeof(EXPR):%d\n", sizeof(EXPR));
      char *expression=(char*)malloc(sizeof(char)*BUFFERSIZE);
      while(expression)
      {
         printf("Please input an expression(just for integer,q for quit):\n");
	     memset(expression,0,BUFFERSIZE);
         gets(expression);
         if(strcmp(expression,"q") == 0)
	     {
    	      break;
         }
	     if(expr_set(expr,expression,strlen(expression)) > -1)
	     {
	          //expr_print(expr);
              fprintf(stdout, "result:%ld\n", expr__test__cal(expr));
	     }
	     expr_clean(expr);
      }
	  free(expression);
      expr_close(expr);
    }
}
#endif
