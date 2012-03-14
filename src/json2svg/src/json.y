%{
	#include<stdio.h>
	#include<string.h>
	#include<stdlib.h>
	#include<stdint.h>
	#define YYSTYPE char*
	#include "json.h"
	char *strconcat(char *str1, char *str2);

	static value *jsonTree;
//TODO: list are only singly linked!!!

#define CHECK_ALLOC(ptr) { if (!ptr) { fprintf (stderr,"ALLOC FAILED AT %d\n", __LINE__); exit (1); } }
	static FILE *logFile;
%}

%token NUMBER
%token STRING
%token TRUE_TOKEN FALSE_TOKEN NULL_TOKEN
%left O_BEGIN O_END A_BEGIN A_END
%left COMMA
%left COLON
%%
START: VALUE {
		jsonTree = (value *) $1;
		fprintf(logFile, "done parsing\n");
	}
	;

OBJECT: O_BEGIN O_END {
		fprintf (logFile, "NULL object\n");
		$$ = NULL;
	}
	| O_BEGIN MEMBERS O_END {
		fprintf (logFile, "object %p\n", $2);
		$$ = $2; 
	}
	;

MEMBERS: PAIR {
		fprintf (logFile, "members final\n");
		members *m = (members *) calloc (sizeof(members), 1);
		CHECK_ALLOC(m);
		m->pair = (pair *) $1;
		m->next = NULL;
		$$ = (char *) m;
	}
	| PAIR COMMA MEMBERS {
		members *m = (members *) calloc (sizeof(members), 1);
		CHECK_ALLOC(m);
		m->pair = (pair *) $1;
		m->next = (members *) $3;
		m->next->prev = m;
		fprintf (logFile, "members not final%p\n", m);
		$$ = (char *) m;

	}
	;

PAIR: STRING COLON VALUE {
		fprintf (logFile, "pair with: %s\n", $1);
		pair *p = (pair *) calloc (sizeof(pair), 1);
		CHECK_ALLOC(p);
		p->str = $1;
		p->val = (value *) $3;
		$$ = (char *) p;
	}
	;

ARRAY: A_BEGIN A_END {
		fprintf (logFile, "NULL array\n");
		$$ = NULL;
	}
	| A_BEGIN ELEMENTS A_END {
		fprintf (logFile, "array\n");
		$$ = $2;
	}
	;

ELEMENTS: VALUE {
		fprintf (logFile, "element 1\n");
		elements *el = calloc (sizeof(elements), 1);
		CHECK_ALLOC(el);
		el->next = NULL; 
		el->val = (value *) $1;
		$$ = (char *) el;
	}
	| VALUE COMMA ELEMENTS {
		fprintf (logFile, "element 2\n");
		elements *el = calloc (sizeof(elements), 1);
		CHECK_ALLOC(el);
		el->next = (elements *) $3;
		el->next->prev = el;
		el->val = (value *) $1;
		$$ = (char *) el;
	}
;

VALUE: 	  STRING { value *val = (value *) calloc (sizeof(value), 1); CHECK_ALLOC(val);
			$$=(char *) val; val->type = VAL_STRING; fprintf (logFile, "string %s\n", yylval);
			val->val.str = (char *) malloc (strlen(yylval)); strcpy (val->val.str, yylval);  }
	| NUMBER { value *val = (value *) calloc (sizeof(value), 1);  CHECK_ALLOC(val);
			$$=(char *) val; val->type = VAL_NUMBER; fprintf (logFile, "number %s\n", yylval);
			sscanf(yylval,"%lld",(long long int *) &val->val.num); }
	| OBJECT { value *val = (value *) calloc (sizeof(value), 1);  CHECK_ALLOC(val);
			$$=(char *) val; val->type = VAL_OBJECT; fprintf (logFile, "object val %p\n", $1);
			val->val.obj = (object) $1; } 
	| ARRAY { value *val = (value *) calloc (sizeof(value), 1);  CHECK_ALLOC(val);
			$$=(char *) val; val->type = VAL_ARRAY; 
			val->val.arr = (array) $1; } 
	| TRUE_TOKEN { value *val = (value *) calloc (sizeof(value), 1);  CHECK_ALLOC(val);
			$$=(char *) val; val->type = VAL_TRUE; fprintf (logFile, "true\n"); }
	| FALSE_TOKEN { value *val = (value *) calloc (sizeof(value), 1);  CHECK_ALLOC(val);
			$$=(char *) val; val->type = VAL_FALSE; fprintf (logFile, "false\n"); }
	| NULL_TOKEN { value *val = (value *) calloc (sizeof(value), 1);  CHECK_ALLOC(val);
			$$=(char *) val; val->type = VAL_NULL; fprintf (logFile, "null\n"); }
	;

%%

int main()
{
	volatile int n = 1;
//	if ((yacc_file != stderr) && (yacc_file != stdout)){ todo: fixme
		logFile = fopen (YACC_FILE, "w");
//	}

	yyparse();

//	if ((yacc_file != stderr) && (yacc_file != stdout)){ todo: fixme
	fclose(logFile);
//	}
	
	work(jsonTree);
	return 0;
}

int yywrap()
{
	return 1;
}

char *strconcat(char *str1, char *str2)
{
	int len1 = strlen(str1);
	int len2 = strlen(str2);
	char *str3 = (char *)malloc(sizeof(char)*(len1+len2+1));
	strcpy(str3,str1);
	strcpy(&(str3[len1]),str2);
	return str3;
}

yyerror (char *s)
{
	fprintf (stderr, "ERROR: %s\n", s);
}

