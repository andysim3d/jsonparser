#include "adjson.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>

#ifndef AD_PARSE_STACK_INIT_SIZE
#define AD_PARSE_STACK_INIT_SIZE 256
#endif


typedef struct {
	const char *json;
	char * stack;
	size_t size, top;
}ad_context;

static void* ad_context_push(ad_context *c, size_t size){
	void* ret;
	assert(size > 0);
	if (c->top + size >= c->size){
		if (c->size == 0)
			c->size = AD_PARSE_STACK_INIT_SIZE;
		while(c->top + size >= c->size){
			c->size += c->size >> 1;
		}
		c->stack = (char*) realloc(c->stack, c->size);
	}
	ret = c->stack + c->top;
	c->top += size;
	return ret;
}

static void* ad_context_pop(ad_context *c, size_t size){
	assert(c->top >= size);
	return c->stack + (c->top -= size);
}

#define EXPECT(c, ch) do { assert(*c->json == (ch)); c->json ++;} while(0)
#define ISDIGIT(ch) ((ch) >= '0' &&(ch) <= '9')
#define ISDIGIT1TO9(ch) ((ch) >= '1' &&(ch) <= '9')
#define ISDOT(ch) ((ch) == '.')
#define ISE(ch) ((ch) == 'E' || (ch) == 'e')
#define ISPOSITIVE(ch) ((ch) == '+')
#define ISNEGATIVE(ch) ((ch) == '-')

static void ad_parse_whitespace(ad_context* c){
	const char *p = c->json;
	while( *p == ' ' || *p == '\t' || *p == 'r' || *p == '\n')
		p++;
	c->json = p;	
}
void ad_free(ad_value *v){
	assert(v != NULL);
	if(v->type == AD_STRING){
		if (v->u.s.value != NULL){
			free(v->u.s.value);
		}	
	v->type = AD_NULL;
	}
	
	v->u.s.len = 0;
}

void ad_set_string(ad_value *v, const char*s, size_t len){
	assert(v != NULL && (s != NULL || len == 0));
	ad_free(v);
	v->u.s.value = (char*)malloc(len + 1);
	memcpy(v->u.s.value, s, len);
	v->u.s.len = len;
	v->u.s.value[len] = '\0';
	v->type = AD_STRING;		
}

/*
	state-->0 init
		
		1 number(without e, dot)
		2 dot
		3 number with dot
		4 e 
		5 number with e
		6 dot after e
		7 number after e after dot
		8 start with 0
		10 negitave after e
		11 positave after e
*/
int 
ad_parse_literal(ad_context *c, ad_value *v, const char* com, int length, int match_value){
	int rc = 0;

	rc = strncmp(c->json, com, length);
	if (rc != 0){
		return AD_PARSE_INVALID_VALUE;
	}
	c->json += length;
	v->type = match_value;
	return AD_PARSE_OK;

}


static int ad_parse_number(ad_context *c, ad_value *v){
	const char *p = c->json;
	if (*p == '-') p++;
	if (*p == '0') p++;
	else{
		if (!ISDIGIT1TO9(*p)) return AD_PARSE_INVALID_VALUE;
		for (p ++; ISDIGIT(*p); p++);
	}
	if (*p == '.'){
		p ++;
		if (!ISDIGIT(*p)) return AD_PARSE_INVALID_VALUE;
		for (p ++; ISDIGIT(*p); p++);
	}
	if (*p == 'e' || *p == 'E' ){
		p++;
		if (*p == '+' || *p == '-') p++;
		if (!ISDIGIT(*p)) return AD_PARSE_INVALID_VALUE;
		for (p ++; ISDIGIT(*p); p++);
	}

	errno = 0;
	v->u.n = strtod(c->json, NULL);
	v->type = AD_NULL;
	if (errno == ERANGE && (v->u.n == HUGE_VAL || v->u.n == -HUGE_VAL)){
		return AD_PARSE_NUMBER_TOO_BIG;
	}
	
	c->json = p;
	v->type = AD_NUMBER;
	return AD_PARSE_OK;
}


int 
ad_get_boolean(const ad_value* v){
	assert( v!= NULL &&(v->type == AD_TRUE || v->type == AD_FALSE));
	return v->type == AD_TRUE;
}

void 
ad_set_boolean(ad_value* v, int b){
	ad_free(v);
	if(b){
		v->type = AD_TRUE;
	}
	else{
		v->type = AD_FALSE;
	}
}



double 
ad_get_number(const ad_value* v) {
	assert(v != NULL && v->type == AD_NUMBER);
	return v->u.n;
}

void 
ad_set_number(ad_value* v, double n){
	ad_free(v);
	v->type = AD_NUMBER;
	v->u.n = n;
}

const char* 
ad_get_string(const ad_value* v){
	assert(v != NULL && v->type == AD_STRING);
	return v->u.s.value;	
}

size_t ad_get_string_length(const ad_value* v){
	assert(v != NULL && v->type == AD_STRING);
	return v->u.s.len;
}


int 
ad_parse_null(ad_context * c, ad_value *v){
	return ad_parse_literal(c, v, "null", 4, AD_NULL);
}

int 
ad_parse_true(ad_context *c, ad_value *v){
	return ad_parse_literal(c, v, "true", 4, AD_TRUE);
}

int
ad_parse_false(ad_context *c, ad_value *v){
	return ad_parse_literal(c, v, "false", 5, AD_FALSE);
}

#define PUTC(c, ch) do{*(char*)ad_context_push(c, sizeof(char)) = (ch);} while(0)

int 
ad_parse_string(ad_context *c, ad_value *v){
	size_t head = c->top, len;
	const char* p;
	EXPECT(c, '\"');
	p = c->json;
#ifdef DEBUG 
	printf("%s\n", p);
#endif
	for(;;){
		char ch = *p ++;
		switch(ch){
			case '\"':
				len = c->top - head;
#ifdef DEBUG 
	printf("len is %d\n", (int)len);
#endif
				ad_set_string(v, (const char*)ad_context_pop(c, len), len);
				c->json = p;
				return AD_PARSE_OK;
		        case '\\':
		                switch (*p++) {
		                    case '\"': PUTC(c, '\"'); break;
		                    case '\\': PUTC(c, '\\'); break;
		                    case '/':  PUTC(c, '/' ); break;
		                    case 'b':  PUTC(c, '\b'); break;
		                    case 'f':  PUTC(c, '\f'); break;
		                    case 'n':  PUTC(c, '\n'); break;
		                    case 'r':  PUTC(c, '\r'); break;
              		  	    case 't':  PUTC(c, '\t'); break;
		                    default:
                		        c->top = head;
		                        return AD_PARSE_INVALID_STRING_ESCAPE;
                		}
				break;
			case '\0':
				c->top = head;
				return AD_PARSE_MISS_QUOTATION_MARK;
			default:
				if ((unsigned int)ch < 0x20){
					c->top = head;
					return AD_PARSE_INVALID_STRING_CHAR;
				}
				PUTC(c,ch);
	
		}

	}

}


int 
ad_parse_value(ad_context * c, ad_value *v){
	switch(*c->json){
		case 'n' : return ad_parse_literal(c,v,"null", 4, AD_NULL);
		case 't' : return ad_parse_literal(c,v,"true", 4, AD_TRUE);
		case 'f' : return ad_parse_literal(c,v,"false", 5, AD_FALSE);
		case '"' : return ad_parse_string(c,v);
		default : return ad_parse_number(c, v);
		case '\0' : return AD_PARSE_EXPECT_VALUE;

	}
}


int
ad_parse(ad_value *v, const char* json){
        ad_context c;
	int ret = 0;
        assert( v != NULL);
        c.json = json;
	c.stack = NULL;
	c.size = c.top = 0;
	ad_init(v);
        ad_parse_whitespace(&c);
        if( (ret = ad_parse_value(&c, v)) == AD_PARSE_OK){
		ad_parse_whitespace(&c);
		if (*c.json != '\0'){
			v->type = AD_NULL;
			ret = AD_PARSE_ROOT_NOT_SINGULAR;
		}
	}
	assert(c.top == 0);
	free(c.stack);
	return ret;
}

ad_type
ad_get_type(const ad_value* v){
	assert(v != NULL);
        return v->type;
}


