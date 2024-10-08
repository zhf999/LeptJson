//
// Created by zhouhf on 2024/10/8.
//

#include "leptjson.h"
#include <assert.h>  /* assert() */
#include <math.h>
#include <stdlib.h>  /* NULL */

#define EXPECT(c, ch)       do { assert(*c->json == (ch)); c->json++; } while(0)
#define ISDIGIT(ch)         ((ch) >= '0' && (ch) <= '9')
#define ISDIGIT1TO9(ch)     ((ch) >= '1' && (ch) <= '9')

typedef struct {
    const char* json;
}lept_context;

static void lept_parse_whitespace(lept_context* c) {
    const char *p = c->json;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
        p++;
    c->json = p;
}

static int lept_parse_literal(lept_context* c, lept_value* v, const char* literal, lept_type type) {
    size_t i;
    EXPECT(c, literal[0]);
    for (i = 0; literal[i + 1]; i++)
        if (c->json[i] != literal[i + 1])
            return LEPT_PARSE_INVALID_VALUE;
    c->json += i;
    v->type = type;
    return LEPT_PARSE_OK;
}

static int lept_validate_number(lept_context* c,const char** p) {
    const char* c_index = c->json;
    if(*c_index=='-')c_index++;
    if(*c_index=='0') c_index++;
    else {
        if(!ISDIGIT1TO9(*c_index))return LEPT_PARSE_INVALID_VALUE;
        while (ISDIGIT(*c_index))c_index++;
    }
    if(*c_index=='.') {
        c_index++;
        if(!ISDIGIT(*c_index))return  LEPT_PARSE_INVALID_VALUE;
        while(ISDIGIT(*c_index))c_index++;
    }
    if(*c_index=='e'||*c_index=='E') {
        c_index++;
        if (*c_index == '+' || *c_index == '-') c_index++;
        if (!ISDIGIT(*c_index)) return LEPT_PARSE_INVALID_VALUE;
        while(ISDIGIT(*c_index))c_index++;
    }
    *p = c_index;
    return LEPT_PARSE_OK;
}

static int lept_parse_number(lept_context* c, lept_value* v) {

    char *end;
    int check_result = lept_validate_number(c,&end);
    if(check_result!=LEPT_PARSE_OK)
        return check_result;
    errno = 0;
    v->n = strtod(c->json,NULL);
    if (errno == ERANGE && (v->n == HUGE_VAL || v->n == -HUGE_VAL))
        return LEPT_PARSE_NUMBER_TOO_BIG;
    v->type = LEPT_NUMBER;
    c->json = end;
    return LEPT_PARSE_OK;
}

static int lept_parse_value(lept_context* c, lept_value* v) {
    switch (*c->json) {
        case 'f': return lept_parse_literal(c,v,"false",LEPT_FALSE);
        case 't': return lept_parse_literal(c,v,"true",LEPT_TRUE);
        case 'n':  return lept_parse_literal(c, v,"null",LEPT_NULL);
        case '\0': return LEPT_PARSE_EXPECT_VALUE;
        default:   return lept_parse_number(c,v);
    }
}

int lept_parse(lept_value* v, const char* json) {
    lept_context c;
    assert(v != NULL);
    c.json = json;
    v->type = LEPT_NULL;
    lept_parse_whitespace(&c);
    int ret;
    if ((ret = lept_parse_value(&c, v)) == LEPT_PARSE_OK) {
        lept_parse_whitespace(&c);
        if (*c.json != '\0') {
            v->type = LEPT_NULL;
            ret = LEPT_PARSE_ROOT_NOT_SINGULAR;
        }

    }
    return ret;
}

lept_type lept_get_type(const lept_value* v) {
    assert(v != NULL);
    return v->type;
}

double lept_get_number(const lept_value* v) {
    assert(v != NULL && v->type == LEPT_NUMBER);
    return v->n;
}

