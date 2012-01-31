/*

I hereby place this program into the public domain.
In case this is not legally possible, the following license applies:

Copyright (C) 2012 Ľubomír Remák

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include <libintl.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <locale.h>

#define LIB_NAME "gettext"
#define DOMAIN_REGISTRY_NAME "gettext_domain"

struct locale_category
{
    int code;
    const char * name;
};

static struct locale_category locale_categories[] = {
    {LC_CTYPE, "LC_CTYPE"},
    {LC_NUMERIC, "LC_NUMERIC"},
    {LC_TIME, "LC_TIME"},
    {LC_COLLATE, "LC_COLLATE"},
    {LC_MONETARY, "LC_MONETARY"},
    {LC_MESSAGES, "LC_MESSAGES"},
    {LC_ALL, "LC_ALL"},
    {LC_PAPER, "LC_PAPER"},
    {LC_NAME, "LC_NAME"},
    {LC_ADDRESS, "LC_ADDRESS"},
    {LC_TELEPHONE, "LC_TELEPHONE"},
    {LC_MEASUREMENT, "LC_MEASUREMENT"},
    {LC_IDENTIFICATION, "LC_IDENTIFICATION"}
};

#define SIZE(x) (sizeof(x)/sizeof(x[0]))

static void set_domain_name(lua_State * L, const char * domain)
{
    lua_pushliteral(L, DOMAIN_REGISTRY_NAME);
    lua_pushstring(L, domain);
    lua_rawset(L, LUA_REGISTRYINDEX);
}

static const char * get_domain_name(lua_State * L)
{
    lua_pushliteral(L, DOMAIN_REGISTRY_NAME);
    lua_rawget(L, LUA_REGISTRYINDEX);
    const char * str = lua_tostring(L, -1);
    char * ret = "";

    if (str != NULL) {
        ret = malloc(strlen(str) + 1);
        strcpy(ret, str);
    }

    lua_pop(L, 1);
    return ret;
}

static int lua_textdomain(lua_State * L)
{
    /* We cannot call textdomain() here. */

    const char * domain = NULL;

    if (lua_isstring(L, -1)) {
        if ((domain = luaL_checkstring(L, -1)) != NULL) {
            set_domain_name(L, domain);
            lua_pushstring(L, domain);

            return 1;
        } else {
            return 0;
        }
    } else {
        lua_pushstring(L, get_domain_name(L));
        return 1;
    }

    return 0;
}

static int lua_bindtextdomain(lua_State * L)
{
    const char * domain = luaL_checkstring(L, -2);
    const char * dir = luaL_checkstring(L, -1);

    if (domain != NULL) {
        if (domain[0] == '\0') {
            luaL_error(L, "attempt to call bindtextdomain with empty domain");
            return 0;
        }

        lua_pushstring(L, bindtextdomain(domain, dir));

        return 1;
    }

    return 0;
}

static int lua_gettext(lua_State * L)
{
    const char * msgid = NULL;

    if ((msgid = luaL_checkstring(L, -1)) != NULL) {
        const char * domain = get_domain_name(L);
        lua_pushstring(L, dgettext(domain, msgid));

        return 1;
    }

    return 0;
}

static int lua_dgettext(lua_State * L)
{
    const char * domain = luaL_checkstring(L, -2);
    const char * msgid = luaL_checkstring(L, -1);

    if (domain != NULL && msgid != NULL) {
        lua_pushstring(L, dgettext(domain, msgid));
        return 1;
    }

    return 0;
}

static int lua_dcgettext(lua_State * L)
{
    const char * domain = luaL_checkstring(L, -3);
    const char * msgid = luaL_checkstring(L, -2);
    int cat = luaL_checkint(L, -1);

    if (domain != NULL && msgid != NULL) {
        lua_pushstring(L, dcgettext(domain, msgid, cat));
        return 1;
    }

    return 0;
}

static int lua_ngettext(lua_State * L)
{
    const char * msgid = luaL_checkstring(L, -3);
    const char * msgid_plural = luaL_checkstring(L, -2);
    int n = luaL_checkint(L, -1);

    if (msgid != NULL && msgid_plural != NULL) {
        const char * domain = get_domain_name(L);
        lua_pushstring(L, dngettext(domain, msgid, msgid_plural, n));

        return 1;
    }

    return 0;
}

static int lua_dngettext(lua_State * L)
{
    const char * domain = luaL_checkstring(L, -4);
    const char * msgid = luaL_checkstring(L, -3);
    const char * msgid_plural = luaL_checkstring(L, -2);
    int n = luaL_checkint(L, -1);

    if (domain != NULL && msgid != NULL && msgid_plural != NULL) {
        lua_pushstring(L, dngettext(domain, msgid, msgid_plural, n));

        return 1;
    }

    return 0;
}

static int lua_dcngettext(lua_State * L)
{
    const char * domain = luaL_checkstring(L, -5);
    const char * msgid = luaL_checkstring(L, -4);
    const char * msgid_plural = luaL_checkstring(L, -3);
    int n = luaL_checkint(L, -2);
    int cat = luaL_checkint(L, -1);

    if (domain != NULL && msgid != NULL && msgid_plural != NULL) {
        lua_pushstring(L, dcngettext(domain, msgid, msgid_plural, n, cat));

        return 1;
    }

    return 0;
}

static luaL_Reg gettext_functions[] = {
    {"bindtextdomain", lua_bindtextdomain},
    {"textdomain", lua_textdomain},
    {"gettext", lua_gettext},
    {"dgettext", lua_dgettext},
    {"dcgettext", lua_dcgettext},
    {"ngettext", lua_ngettext},
    {"dngettext", lua_dngettext},
    {"dcngettext", lua_dcngettext},

    {NULL, NULL},
};

void luaopen_gettext(lua_State * L)
{
    setlocale(LC_ALL, "");

    luaL_register(L, LIB_NAME, gettext_functions);

    int i;

    for (i = 0; i < SIZE(locale_categories); i++) {
        lua_pushstring(L, locale_categories[i].name);
        lua_pushinteger(L, locale_categories[i].code);
        lua_settable(L, -3);
    }

    lua_pop(L, 1);
}

