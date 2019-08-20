
#include "postgres.h"
#include "fmgr.h"
#include "libpq/pqformat.h"
#include <string.h>
#include "access/hash.h"
PG_MODULE_MAGIC;

typedef struct EmailAddr
{
    int32 length;
    int32 domain_index;     //index of domain
    char addr[FLEXIBLE_ARRAY_MEMBER];
} emailaddr;

int32 isValidChar(char c);
int32 validator(char *string);
int32 email_local_cmp(emailaddr * a, emailaddr * b);
int32 email_domain_cmp(emailaddr * a, emailaddr * b);
Datum email_in(PG_FUNCTION_ARGS);
Datum email_out(PG_FUNCTION_ARGS);


Datum email_lt(PG_FUNCTION_ARGS);
Datum email_le(PG_FUNCTION_ARGS);
Datum email_eq(PG_FUNCTION_ARGS);
Datum email_ne(PG_FUNCTION_ARGS);
Datum email_ge(PG_FUNCTION_ARGS);
Datum email_gt(PG_FUNCTION_ARGS);


// Verify if the characters meet the reqifuirements

int32 isValidChar(char c){
    if (c == '-' || c == '@' || c == '.'|| isalpha(c) || isdigit(c))  return 1;
    else return 0;
}

// Verify if the email address meets the requirements
int32 validator(char *string){
    int32 status = 0;    // when checking local status == 0, checking domain status == 1;
    int32 dotCounter = 0;// '.' counter
    int32 i = 0;         // index
    int32 domain_index = 0; //index of domain
    if(!(isalpha(string[i])) ){
        return 1;
    }
    for (i = 1; string[i]; i++) {
        //Verify if the characters meet the reqifuirements
        if (!isValidChar(string[i])){
            return 1;
        }
        if ((string[i-1] == '.' || string[i-1] == '@') &&!isalpha(string[i])) {
            return 1;
        }
        if ((string[i] == '.' || string[i] == '@') &&!(isalpha(string[i-1]) || isdigit(string[i-1]))){
            return 1;
        }
        
        if (string[i] == '@') {
            //checking lcoal length and end char
            if ( status == 1||i > 256) {return 1;}
            status = 1;
            domain_index = i+1;
        }
        //checking '.'
        if (status && (string[i] == '.')) {
            dotCounter++;
        }
    }
    //checking domain length and end char
    if (i-domain_index>256||!(isalpha(string[i-1]) || isdigit(string[i-1]))||dotCounter < 1){
        return 1;
    }
    return 0;
}
// IN function
PG_FUNCTION_INFO_V1(email_in);
Datum email_in(PG_FUNCTION_ARGS){
    char *str = PG_GETARG_CSTRING(0);
    int32 length = strlen(str);
    int32 status = validator(str);
    emailaddr *result;
    int32 at_len = 0;
    if (status == 1 && length !=0){
        //report erroe when email is invaild
        ereport( ERROR, (errcode (ERRCODE_INVALID_TEXT_REPRESENTATION),errmsg ("invalid input syntax for EmailAdd: \"%s\"", str)));
    }else{
        
        //Lowercase()
	int32 i;
        for (i = 0;str[i];i++){
            if (str[i] == '@'){
                at_len = i;
            }
            if (str[i]>= 'A'&& str[i]<= 'Z'){
                str[i] = str[i]^32;
            }
        }
    }
    //allocate memory
    result = (emailaddr *) palloc(VARHDRSZ +sizeof(int32)+length+1 * sizeof(char));
    SET_VARSIZE(result,VARHDRSZ +sizeof(int32)+(length+1)* sizeof(char));
    memcpy(result -> addr, str, length * sizeof(char));
    result -> addr[length] = '\0';
    result -> domain_index = at_len;
    PG_RETURN_POINTER(result);
}


// OUT function
PG_FUNCTION_INFO_V1(email_out);
Datum email_out(PG_FUNCTION_ARGS)
{
    emailaddr  *addr = (emailaddr *) PG_GETARG_POINTER(0);
    char *result;
    //allocate memory
    int32 length = VARSIZE_ANY_EXHDR(addr)-sizeof(int32);
    result = (char *) palloc(sizeof(char)*length-1);
    snprintf(result, sizeof(char)*length+1, "%s", addr->addr);
    PG_RETURN_CSTRING(result);
}
// compere local part
int32 email_local_cmp(emailaddr * a, emailaddr * b)
{
    int32 r;
    // strcmp end at '\0', so replace '@' with '\0'
    a-> addr[a->domain_index] = '\0';
    b-> addr[b->domain_index] = '\0';
    r = strcmp(a -> addr,b -> addr);
    a-> addr[a->domain_index] = '@';
    b-> addr[b->domain_index] = '@';
    return r;
}

/*
int32 email_local_cmp(emailaddr * a, emailaddr * b)
{
    int32 r;
    int32 len;
    if (a->domain_index > b->domain_index){
        len = b->domain_index;
    }else{
        len = a->domain_index;
    }
    r = memcmp(a -> addr,b -> addr,len);
    if (r == 0&&a->domain_index != b->domain_index){
        if (a->domain_index > b->domain_index){
            r = 1;
        }else{
            r = -1;
        }
    }
    return r;
}*/

// compere domain part
int32 email_domain_cmp(emailaddr * a, emailaddr * b)
{
    int32 r;
    //get lenght of domain parts
    int32 len1 = VARSIZE_ANY_EXHDR(a)-sizeof(int32) - a->domain_index-1;
    int32 len2 = VARSIZE_ANY_EXHDR(b)-sizeof(int32) - b->domain_index-1;
    int32 len;
    // len = min(len1,len2)
    if (len1>len2){
        len = len2;
    } else {
        len = len1;
    }
    r = strcmp(&a -> addr[a -> domain_index + 1],&b -> addr[b -> domain_index + 1]);
    if(r == 0&&(len1!=len2)){
        if (len1 < len2){
            r = -1;
        } else {
            r = 1;
        }
    }
    return r;
}
/*
int32 email_domain_cmp(emailaddr * a, emailaddr * b)
{
    int32 r;
    
    int32 len1 = VARSIZE_ANY_EXHDR(a)-sizeof(int32) - a->domain_index-1;
    int32 len2 = VARSIZE_ANY_EXHDR(b)-sizeof(int32) - b->domain_index-1;
    int32 len;
    if (len1>len2){
        len = len2;
    } else {
        len = len1;
    }
    r = memcmp(&a -> addr[a -> domain_index + 1],&b -> addr[b -> domain_index + 1],len);
    if(r == 0&&(len1!=len2)){
        if (len1 < len2){
            r = -1;
        } else {
            r = 1;
        }
    }
    return r;
}*/

// < operation
PG_FUNCTION_INFO_V1(email_lt);
Datum email_lt(PG_FUNCTION_ARGS)
{
    emailaddr    *a = (emailaddr *) PG_GETARG_POINTER(0);
    emailaddr    *b = (emailaddr *) PG_GETARG_POINTER(1);
    bool result;
    // first compere domain
    int32 domain = email_domain_cmp(a,b);
    if (domain < 0){
        result = true;
    }else if (domain == 0){
        // then compere local
        int32 local =email_local_cmp(a,b);
        if(local < 0){
            result = true;
        }else{
            result = false;
        }
    }else{
        result = false;
    }
    PG_RETURN_BOOL(result);
}
PG_FUNCTION_INFO_V1(email_le);
Datum email_le(PG_FUNCTION_ARGS)
{
    emailaddr    *a = (emailaddr *) PG_GETARG_POINTER(0);
    emailaddr    *b = (emailaddr *) PG_GETARG_POINTER(1);
    bool result;
    // first compere domain
    int32 domain = email_domain_cmp(a,b);
    if (domain <= 0){
        result = true;
    }else if (domain == 0){
        // then compere local
        int32 local =email_local_cmp(a,b);
        if(local <= 0){
            result = true;
        }else{
            result = false;
        }
    }else{
        result = false;
    }
    PG_RETURN_BOOL(result);
}
// = operation
PG_FUNCTION_INFO_V1(email_eq);
Datum email_eq(PG_FUNCTION_ARGS)
{
    emailaddr    *a = (emailaddr *) PG_GETARG_POINTER(0);
    emailaddr    *b = (emailaddr *) PG_GETARG_POINTER(1);
    bool result;
   /* int32 len1 = VARSIZE_ANY_EXHDR(a)-sizeof(int32);
    int32 len2 = VARSIZE_ANY_EXHDR(a)-sizeof(int32);
   int i;
    if (len1!=len2){
        result = false;
    }else{
        i = memcmp(a -> addr,b -> addr,len1);
        if(i == 0){
            result = true;
        }else{
            result = false;
        }
    }*/
    int i = strcmp(a->addr,b->addr);
    if(i==0){
    	result = true;
    }else{
	result = false;
    }
    PG_RETURN_BOOL(result);
}
PG_FUNCTION_INFO_V1(email_ne);
Datum email_ne(PG_FUNCTION_ARGS)
{
    emailaddr    *a = (emailaddr *) PG_GETARG_POINTER(0);
    emailaddr    *b = (emailaddr *) PG_GETARG_POINTER(1);
    bool result;
   /* int i;
    int32 len1 = VARSIZE_ANY_EXHDR(a)-sizeof(int32);
    int32 len2 = VARSIZE_ANY_EXHDR(b)-sizeof(int32);;
    if (len1!=len2){
        result = true;
    }else{
        i = memcmp(&a -> addr,&b -> addr,len1);
        if(i == 0){
            result = false;
        }else{
            result = true;
        }
    }*/
    int i = strcmp(a->addr,b->addr);
    if(i==0){
        result =false;
    }else{
        result = true;
    }   


    PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(email_ge);
Datum email_ge(PG_FUNCTION_ARGS)
{
    emailaddr    *a = (emailaddr *) PG_GETARG_POINTER(0);
    emailaddr    *b = (emailaddr *) PG_GETARG_POINTER(1);
    bool result;
    int32 domain = email_domain_cmp(a,b);
    if (domain >= 0){
        result = true;
    }else if (domain == 0){
        int local =email_local_cmp(a,b);
        if(local >= 0){
            result = true;
        }else{
            result = false;
        }
    }else{
        result = false;
    }
    PG_RETURN_BOOL(result);
}
PG_FUNCTION_INFO_V1(email_gt);
Datum email_gt(PG_FUNCTION_ARGS)
{
    emailaddr    *a = (emailaddr *) PG_GETARG_POINTER(0);
    emailaddr    *b = (emailaddr *) PG_GETARG_POINTER(1);
    bool result;
    int domain = email_domain_cmp(a,b);
    if (domain > 0){
        result = true;
    }else if (domain == 0){
        int local =email_local_cmp(a,b);
        if(local > 0){
            result = true;
        }else{
            result = false;
        }
    }else{
        result = false;
    }
    PG_RETURN_BOOL(result);
}


PG_FUNCTION_INFO_V1(email_de);
Datum email_de(PG_FUNCTION_ARGS)
{
    emailaddr    *a = (emailaddr *) PG_GETARG_POINTER(0);
    emailaddr    *b = (emailaddr *) PG_GETARG_POINTER(1);
    bool result;
    int domain = email_domain_cmp(a,b);
    if (domain == 0){
        result =true;
    }else{
        result = false;
    }
    PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(email_dne);
Datum email_dne(PG_FUNCTION_ARGS)
{
    emailaddr    *a = (emailaddr *) PG_GETARG_POINTER(0);
    emailaddr    *b = (emailaddr *) PG_GETARG_POINTER(1);
    bool result;
    int domain = email_domain_cmp(a,b);
    if (domain == 0){
        result =true;
    }else{
        result = false;
    }
    PG_RETURN_BOOL(!result);
}

PG_FUNCTION_INFO_V1(email_hash);
Datum email_hash(PG_FUNCTION_ARGS)
{
    emailaddr *a = (emailaddr *) PG_GETARG_POINTER(0);
    //get hash value of string, by using hash_any() in postgre
    uint32 hash =  hash_any((unsigned char *) a->addr,VARSIZE_ANY_EXHDR(a)-sizeof(int32));
    
    //ereport( WARNING, (errcode (ERRCODE_INVALID_TEXT_REPRESENTATION),errmsg ("hash:%u\n", hash)));
    PG_FREE_IF_COPY(a, 0);
    PG_RETURN_DATUM(hash);
}

PG_FUNCTION_INFO_V1(email_cmp);
Datum email_cmp(PG_FUNCTION_ARGS)
{
    emailaddr    *a = (emailaddr *) PG_GETARG_POINTER(0);
    emailaddr    *b = (emailaddr *) PG_GETARG_POINTER(1);
    int result;
    int32 domain = email_domain_cmp(a,b);
    if (domain > 0){
        result = 1;
    }else if (domain == 0){
        int local =email_local_cmp(a,b);
        if(local == 0){
            result = 0;
        }else if(local > 0){
            result = 1;
        }else{
	    result = -1;	
	}
    }else{
        result = -1;
    }
    PG_RETURN_INT32(result);
}


