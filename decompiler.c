#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define XP_UNIX
#include "jsapi.h"
#include "jsxdrapi.h"

#define CONTEXT_SIZE 8192

#define ERROR -1


int main(int argc, char *argv[])
{
    JSRuntime *rt;
    JSContext *cx;
    JSObject  *glob;
    long       rtsize = 8;      /* Runtime size allocated in MB */
    JSBool     status;
    JSScript  *script;
    jsval      rval;


    JSClass dj_Global = { "global", JSCLASS_HAS_PRIVATE,
                          JS_PropertyStub, JS_PropertyStub,
                          JS_PropertyStub, JS_PropertyStub,
                          JS_EnumerateStub, JS_ResolveStub,
                          JS_ConvertStub,  JS_FinalizeStub,
                          NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0
                        };

    rt = JS_NewRuntime(1024L * 1024L * rtsize);
    if (rt == NULL)
    {
        printf("Cannot initialise javascript runtime\n");
        return ERROR;
    }

/* Create a javascript context and associate it with the JS runtime */
    cx = JS_NewContext(rt, CONTEXT_SIZE);
    if (cx == NULL)
    {
        printf("Cannot initialise javascript context\n");
        return ERROR;
    }

/* Create a global object */
    glob = JS_NewObject(cx, &dj_Global, NULL, NULL);
    if (glob == NULL)
    {
        printf("Cannot create javascript global object\n");
        return ERROR;
    }

/* Create the standard classes */
    status = JS_InitStandardClasses(cx, glob);
    if (status == JS_FALSE)
    {
        printf("Cannot initialise javascript standard classes\n");
        return ERROR;
    }

/* XDR script */
    JSXDRState *xdr = JS_XDRNewMem(cx, JSXDR_DECODE);
    if ((xdr != NULL)) {
      void* f = fopen(argv[1], "rb");
      fseek(f, 0, SEEK_END);
      int len = ftell(f);
      fseek(f, 0, SEEK_SET);
      void* data = malloc(len);
      fread(data, 1, len, f);
      JS_XDRMemSetData(xdr, data, len);
      if (JS_XDRScript(xdr, &script) != JS_TRUE) {
        printf("Decompilation error\n");
      };
    }

/* Decompile script */
    JSString *sourcecode = JS_DecompileScript(cx, script, "proba.js", 2);
    char *sourcecode_str = JS_GetStringBytes(sourcecode);
    printf("%s", sourcecode_str);

/* Destroy context */

    JS_DestroyContext(cx);

/* Destroy runtime */

    JS_DestroyRuntime(rt);

    return 0;
}

