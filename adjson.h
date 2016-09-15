#ifndef ADJSON_H_

#define ADJSON_H_

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
} ad_value;


enum{
	AD_PARSE_OK = 0,
	AD_PARSE_EXPECT_VALUE, 
	AD_PARSE_INVALID_VALUE,
	AD_PARSE_ROOT_NOT_SINGULAR 
};
int ad_parse(ad_value *v, const char* json);

ad_type ad_get_type(const ad_value* v);

#endif 
/*ADJSON_H_*/