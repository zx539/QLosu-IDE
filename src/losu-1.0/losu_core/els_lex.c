#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "els.h"

#include "els_lex.h"
#include "els_mem.h"
#include "els_object.h"
#include "els_parser.h"
#include "els_vmhost.h"
#include "els_string.h"
#include "els_unit.h"

#define next(LS) (LS->current = els_vmio_getc(LS->z))
#define checkbuffer(L, n, len)      \
    if ((len) + (n) > L->BufferTmpSize) \
    els_Object_openspace(L, (len) + (n))

#define save(L, c, l) (L->BufferTmp[l++] = (char)c)
#define save_and_next(L, LS, l) (save(L, LS->current, l), next(LS))

char laststr[128];
int lasttokenid;
struct 
{
    const char* string;
    int key;
}const keyword[] = {
#ifdef ELS_CONF_TOKEN_EN
    {"and", TOKEN_AND},
    {"break", TOKEN_BREAK},
    {"block", TOKEN_BLOCK},
    {"else", TOKEN_ELSE},
    {"elif", TOKEN_ELSEIF},
    {"for", TOKEN_FOR},
    {"def", TOKEN_FUNCTION},
    {"if", TOKEN_IF},
    {"var", TOKEN_VAR},
    {"null", TOKEN_NULL},
    {"not", TOKEN_NOT},
    {"or", TOKEN_OR},
    {"loop", TOKEN_LOOP},
    {"rt", TOKEN_RETURN},
    {"return", TOKEN_RETURN},
    {"to", TOKEN_TO},
    {"with", TOKEN_WITH},
    {"pass", TOKEN_END},
#endif
#ifdef ELS_CONF_TOKEN_CN
    {"且", TOKEN_AND},
    {"跳出", TOKEN_BREAK},
    {"代码块", TOKEN_BLOCK},
    {"否则", TOKEN_ELSE},
    {"另外", TOKEN_ELSEIF},
    {"计次", TOKEN_FOR},
    {"列举", TOKEN_FOR},
    {"方法", TOKEN_FUNCTION},
    {"如果", TOKEN_IF},
    {"令", TOKEN_VAR},
    {"空", TOKEN_NULL},
    {"非", TOKEN_NOT},
    {"或", TOKEN_OR},
    {"重复", TOKEN_LOOP},
    {"回", TOKEN_RETURN},
    {"至", TOKEN_TO},
    {"当", TOKEN_WITH},
    {"结束", TOKEN_END},
    {"此", TOKEN_THIS},
    {"__极值__", TOKEN_FOR_MAX},
    {"__步幅__", TOKEN_FOR_STEP},
    {"__此__", TOKEN_FOR_THIS},
    {"变参", TOKEN_VARARG},
    {"构造",TOKEN_CONST},
#endif

    {";", TOKEN_END},
};

void els_lexer_init(els_VmObj *L)
{
    int i;
    for (i = 0; i < sizeof(keyword) / sizeof(keyword[0]); i++)
    {
        TString *ts = els_string_new(L, keyword[i].string);
        ts->marked = keyword[i].key;
    }
}

void els_lexer_token2str(int token, char *s)
{
    if (token < 256)
    {
        s[0] = (char)(token);
        s[1] = '\0';
        /*
            使用sprintf不是一个很好的选择
            sprintf(s,"%c",(char)token);
        */
    }
    else
    {
        int i;
        for (i = 0; i < sizeof(keyword) / sizeof(keyword[0]); i++)
            if ((int)keyword[i].key == token)
                break;
        ;
        strcpy(s, keyword[i].string);
    }
}

#define inclinenumber(LS) (next(LS), ++LS->linenumber)

void els_lexer_setinput(els_VmObj *L, LexObject *LS, vm_iobuff *z, TString *source)
{
    LS->L = L;
    LS->lookahead.token = TOKEN_EOS;
    LS->z = z;
    LS->fs = NULL;
    LS->linenumber = 1;
    LS->lastline = 1;
    LS->source = source;
    next(LS);
}


static int utf8_code_bytes( unsigned char byte)
{
    int bytes = 0;
    if (byte <= 0x7F) { //then ASCII 占用1个字节
        bytes = 1;
    } else if (byte >= 0xC0 && byte <= 0xDF) {  // then 首字节   UTF-8 占用2个字节
        bytes = 2;
    } else if (byte >= 0xE0 && byte <= 0xEF) {  // then 首字节   UTF-8 占用3个字节
        bytes = 3;
    } else if (byte >= 0xF0 && byte <= 0xF7) {  // then 首字节   UTF-8 占用4个字节
        bytes = 4;
    } else if (byte >= 0xF8 && byte <= 0xFB) {  // then 首字节   UTF-8 占用5个字节
        bytes = 5;
    } else if (byte >= 0xFC && byte <= 0xFD) {  // then 首字节   UTF-8 占用6个字节
        bytes = 6;
    } else if (byte > 0x7F && byte < 0xC0 ) {   // then UTF-8   非首字节
        bytes = 0;
    }

    return bytes;
}

static const char *read_name(LexObject *LS)
{
    els_VmObj *L = LS->L;
    size_t l = 0;
    do
    {
        int le = utf8_code_bytes((unsigned char)(LS->current));
        
        for (int i = 0;i<le;i++){
            checkbuffer(L, 8, l);
            save_and_next(L, LS, l);
        }

    } while (isalnum(LS->current) || LS->current == '_' || utf8_code_bytes((unsigned char)(LS->current)) > 1);
    save(L, '\0', l);
    return L->BufferTmp;
}

static void read_number(LexObject *LS, int comma, SemInfo *seminfo)
{
    char errorinfo[64];
    els_VmObj *L = LS->L;
    size_t l = 0;
    checkbuffer(L, 10, l);
    if (comma)
        save(L, '.', l);
    while (isdigit(LS->current))
    {
        checkbuffer(L, 10, l);
        save_and_next(L, LS, l);
    }
    if (LS->current == '.')
    {
        save_and_next(L, LS, l);
        if (LS->current == '.')
        {
            save_and_next(L, LS, l);
            save(L, '\0', l);
            sprintf(errorinfo, "错误的数字格式");
            els_compiler_error(LS, errorinfo);
        }
    }
    while (isdigit(LS->current))
    {
        checkbuffer(L, 10, l);
        save_and_next(L, LS, l);
    }
    if (LS->current == 'e' || LS->current == 'E')
    {
        save_and_next(L, LS, l);
        if (LS->current == '+' || LS->current == '-')
            save_and_next(L, LS, l);
        while (isdigit(LS->current))
        {
            checkbuffer(L, 10, l);
            save_and_next(L, LS, l);
        }
    }
    save(L, '\0', l);
    if (!els_Object_str2d(L->BufferTmp, &seminfo->r))
    {
        sprintf(errorinfo, "错误的数字格式,在第 %d 行", LS->linenumber);
        els_compiler_error(LS, errorinfo);
    }
}

static int read_string(LexObject *LS, int del, SemInfo *seminfo)
{
    els_VmObj *L = LS->L;
    size_t l = 0;
    checkbuffer(L, 10, l);
    save_and_next(L, LS, l);
    if (LS->current != del)
    {
        
        while (LS->current != del)
        {
            if(LS->current==EOZ)
                els_compiler_error(LS,"字符串溢出");
            if(LS->current=='\n') ++ LS->linenumber;
            checkbuffer(L, 10, l);
            if(LS->current=='%'){
                next(LS);
                int t=0;
                if(!(('0'<= LS->current && LS->current<='9')||('a'<= LS->current && LS->current<='f')||('A'<= LS->current && LS->current<='F') ))
                    els_compiler_error(LS,"转义序列超出范围");
                switch (LS->current)
                {
                    case 'A': case 'a':
                        t = 16*10;
                        break;
                    case 'B': case 'b':
                        t = 16*11;
                        break;
                    case 'C': case 'c':
                        t = 16*12;
                        break;
                    case 'D': case 'd':
                        t = 16*13;
                        break;
                    case 'E': case 'e':
                        t = 16*14;
                        break;
                    case 'F': case 'f':
                        t = 16*15;
                        break;
                
                default:
                    t = 16*(LS->current-'0');
                }
                next(LS);
                if(!(('0'<= LS->current && LS->current<='9')||('a'<= LS->current && LS->current<='f')||('A'<= LS->current && LS->current<='F') ))
                    els_compiler_error(LS,"转义序列超出范围");
                switch (LS->current)
                {
                    case 'A': case 'a':
                        t = t+10;
                        break;
                    case 'B': case 'b':
                        t = t+11;
                        break;
                    case 'C': case 'c':
                        t = t+12;
                        break;
                    case 'D': case 'd':
                        t = t+13;
                        break;
                    case 'E': case 'e':
                        t = t+14;
                        break;
                    case 'F': case 'f':
                        t = t+15;
                        break;
                
                default:
                    t = t+(LS->current-'0');
                }
                save(L,(char)(t),l);
                next(LS);
                continue;

            }
            if (LS->current == '\\')
            {
                next(LS);
                switch (LS->current)
                {
                case 'a':
                    save(L, '\a', l);
                    next(LS);
                    break;
                case 'b':
                    save(L, '\b', l);
                    next(LS);
                    break;
                case 'f':
                    save(L, '\f', l);
                    next(LS);
                    break;
                case 'n':
                    save(L, '\n', l);
                    next(LS);
                    break;
                case 'r':
                    save(L, '\r', l);
                    next(LS);
                    break;
                case 't':
                    save(L, '\t', l);
                    next(LS);
                    break;
                case 'v':
                    save(L, '\v', l);
                    next(LS);
                    break;
                case '"':
                    save(L, '\"', l);
                    next(LS);
                    break;
                case '\'':
                    save(L, '\'', l);
                    next(LS);
                    break;
                case '\n':
                    save(L, '\n', l);
                    inclinenumber(LS);
                    break;
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                {
                    int c = 0;
                    int i = 0;
                    do
                    {
                        c = 10 * c + (LS->current - '0');
                        next(LS);
                    } while (++i < 3 && isdigit(LS->current));
                    if (c != (unsigned char)c)
                    {
                        save(L, '\0', l);
                        els_compiler_error(LS, "转义序列超出范围");
                    }
                    save(L, c, l);
                    break;
                }
                default:
                    save_and_next(L, LS, l);
                }
                continue;
            }
            save_and_next(L, LS, l);
        }
        save_and_next(L, LS, l);
        save(L, '\0', l);
        seminfo->ts = els_string_newlstr(L, L->BufferTmp + 1, l - 3);
        return 0;
    }else {
        save_and_next(L, LS, l);
        if(LS->current!=del){
            save(L, '\0', l);
            seminfo->ts = els_string_newlstr(L, L->BufferTmp + 1, l - 3);
            return 0;
        }else{
            while (1)
            {
                next(LS);
                if(LS->current==del){
                    next(LS);
                    if(LS->current==del){
                        next(LS);
                        if(LS->current==del){
                            next(LS);
                            return 1;
                        }
                    }
                }
                if(LS->current==EOF){
                    els_compiler_error(LS,"多行注释出错");
                }
                if(LS->current=='\n') ++(LS->linenumber);
            }
            
        }

    }

}

int els_lexer_lex(LexObject *LS, SemInfo *seminfo)
{
    while (1)
    {
        switch (LS->current)
        {
        case '$':
            next(LS);
            return TOKEN_VAR;
        case '#':
            while (LS->current != '\n')
                next(LS);
            break;
        case '^':
            next(LS);
            return '^';
        case '|':
            next(LS);
            if (LS->current == '|')
            {
                next(LS);
                return TOKEN_OR;
            }
            else
                return '|';
        case ' ':
        case '\t':
        case '\r':
            next(LS);
            continue;
        case '\n':
            inclinenumber(LS);
            continue;
        case '-':
            next(LS);
            return '-';
        case '[':
            next(LS);
            return '[';
        case '=':
            next(LS);
            if (LS->current != '=')
                return '=';
            else
            {
                next(LS);
                return TOKEN_EQ;
            }
        case '<':
            next(LS);
            if (LS->current != '=')
                return '<';
            else
            {
                next(LS);
                return TOKEN_LE;
            }
        case '>':
            next(LS);
            if (LS->current != '=')
                return '>';
            else
            {
                next(LS);
                return TOKEN_GE;
            }
        case ';':
            next(LS);
            return TOKEN_END;
        case ':':
            next(LS);
            return ':';
        case '!':
            next(LS);
            if (LS->current != '=')
                return TOKEN_NOT;
            else
            {
                next(LS);
                return TOKEN_NE;
            }
        case '"':
        case '\'':
            if (read_string(LS, LS->current, seminfo))
                continue;
            return TOKEN_STRING;
        case '&':
            next(LS);
            if (LS->current == '&')
            {
                next(LS);
                return TOKEN_AND;
            }
            else
                return TOKEN_CONCAT;
        case '.':
            next(LS);
            if (LS->current == '.')
            {
                next(LS);
                if (LS->current == '.')
                {
                    next(LS);
                    return TOKEN_ARG;
                }
                els_compiler_error(LS, "..不是可识别的字符");
            }
            if (!isdigit(LS->current))
                return '.';
            else
            {
                read_number(LS, 1, seminfo);
                return TOKEN_NUMBER;
            }

        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            read_number(LS, 0, seminfo);
            return TOKEN_NUMBER;

        case EOZ:
            return TOKEN_EOS;

        default:
            if (!isalpha(LS->current) && LS->current != '_' && (unsigned int)(LS->current) < 0x80)
            {
                int c = LS->current;
                next(LS);
                return c;
            }
            TString *ts = els_string_new(LS->L, read_name(LS));
            if (ts->marked >= FIRST_TOKENID)
            {
#ifdef ELS_CONF_TOKEN_CN
                switch (ts->marked)
                {
                case TOKEN_THIS:
                {
                    seminfo->ts = els_string_new(LS->L, "this");
                    return TOKEN_NAME;
                }
                case TOKEN_VARARG:
                {
                    seminfo->ts = els_string_new(LS->L, "arg");
                    return TOKEN_NAME;
                }
                case TOKEN_FOR_MAX:
                {
                    seminfo->ts = els_string_new(LS->L, "__MAX__");
                    return TOKEN_NAME;
                }
                case TOKEN_FOR_STEP:
                {
                    seminfo->ts = els_string_new(LS->L, "__STEP__");
                    return TOKEN_NAME;
                }
                case TOKEN_FOR_THIS:
                {
                    seminfo->ts = els_string_new(LS->L, "__THIS__");
                    return TOKEN_NAME;
                }
                case TOKEN_CONST :
                {
                    seminfo->ts = els_string_new(LS->L, "class");
                    return TOKEN_NAME;
                }
                default:
                    break;
                }
#endif
                return ts->marked;
            }
            seminfo->ts = ts;
            return TOKEN_NAME;
        }
    }
}
