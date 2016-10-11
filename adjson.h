#ifndef ADJSON_H_

#define ADJSON_H_

#include <stddef.h>
typedef enum {
	AD_NULL,
	AD_FALSE, 
	AD_TRUE, 
	AD_NUMBER,
	AD_STRING,
	AD_ARRAY,
	AD_OBJECT
} ad_type;

typedef struct {
	ad_type type;
	char * name;
	union{
		double n;
		struct{char * value; size_t len;}s;
	}u;
} ad_value;


enum{
	AD_PARSE_OK = 0,
	AD_PARSE_EXPECT_VALUE, 
	AD_PARSE_INVALID_VALUE,
	AD_PARSE_ROOT_NOT_SINGULAR,
	AD_PARSE_NUMBER_TOO_BIG ,
	AD_PARSE_MISS_QUOTATION_MARK,
	AD_PARSE_INVALID_STRING_ESCAPE,
	AD_PARSE_INVALID_STRING_CHAR
};
int 
ad_parse(ad_value *v, const char* json);

ad_type 
ad_get_type(const ad_value* v);

#define ad_init(v) do \
	{\
	 (v)->type = AD_NULL;\
	} \
	 while(0)


void 
ad_free(ad_value *v);



#define ad_set_null(v) ad_free(v)

double 
ad_get_number(const ad_value *v);

void 
ad_set_number(ad_value *v, double n);

int 
ad_get_boolean(const ad_value *v);

void
ad_set_boolean(ad_value *v, int b);

const char* 
ad_get_string(const ad_value *v);

size_t
ad_get_string_length(const ad_value*v);

void 
ad_set_string(ad_value*v, const char* s, size_t len);





#endif 
/*ADJSON_H_*/
