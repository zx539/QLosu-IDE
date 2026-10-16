#include <regex>
#include <string>

#include "cre.h"

using namespace std;
using namespace __cxx11;

XKIT_API int re_match(const char *targert, const char *p)
{
    smatch results;
    string _t = targert;
    string _p = p;
    return regex_match(_t, results, regex(_p));
}

XKIT_API LsUnit re_search(const char *targert, const char *p, els_VmObj *vm)
{
    smatch results;
    string _t = targert;
    regex pattern(p);
    LsUnit output = obj_newunit(vm);
    string::const_iterator iter = _t.begin();
    string::const_iterator iterEnd = _t.end();

    for (uint64_t i = 0; regex_search(iter, iterEnd, results, pattern);)
    {
        string tmp = results[0];
        i ++;
        obj_setunitbynum(vm, output, i, obj_newstr(vm, (char*)tmp.c_str()));
        iter = results[0].second;
    }

    return output;
}

XKIT_API LsString re_replace(const char *targert, const char *p, const char *replace_string, els_VmObj *vm)
{
    smatch results;
    string _t = targert;
    string _p = p;
    string _r = replace_string;
    string output =  regex_replace(_t, regex(_p), _r);
    return obj_newstr(vm, (char*)output.c_str());
}
