/*
 * tclmark.c --
 *
 *	This file implements a Tcl interface to the cmark Markdown processor.
 *
 * Copyright (c) 2017 Ashok P. Nadkarni
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tcl.h"
#include "cmark.h"
#include "core-extensions.h"

static int TclCMark_RenderObjCmd(ClientData clientData, Tcl_Interp *interp,
                                 int objc, Tcl_Obj *const objv[]);
extern DLLEXPORT int Cmark_Init(Tcl_Interp * interp);

static void TclCMark_MemPanic()
{
    Tcl_Panic("Memory allocation request exceeds limit.");
}

/*
 * Memory allocator to use to allocate rendered result. We use Tcl's 
 * allocator so we can directly use allocated memory as interpreter result.
 * We need wrappers because parameter types differ (int v/s size_t).
 */
void *TclCMark_calloc(size_t nmem, size_t size)
{
    size_t nbytes;
    void *p;
    if (nmem > INT_MAX || size > INT_MAX)
        TclCMark_MemPanic();

    nbytes = nmem * size;       /* Cannot overflow because of above checks */
    if (nbytes > INT_MAX)
        TclCMark_MemPanic();

    p = ckalloc((int)nbytes);   /* Panics on failure to allocate */
    memset(p, 0, nbytes);
    return p;
}

void *TclCMark_realloc(void *p, size_t size)
{
    if (size > INT_MAX)
        TclCMark_MemPanic();
    return ckrealloc(p, (int) size);
}

void TclCMark_free(void *p)
{
    ckfree(p);
}

static cmark_mem TclCMark_Allocator = {
    TclCMark_calloc, TclCMark_realloc, TclCMark_free
};

/*
 * render MARKDOWNTEXT
 *
 * Renders markdown text in the specified format, returning it as the
 * interp result.
 */

static int TclCMark_RenderObjCmd(
    ClientData clientData,	/* Not used. */
    Tcl_Interp *interp,		/* Current interpreter */
    int objc,			/* Number of arguments */
    Tcl_Obj *const objv[]	/* Argument strings */
    )
{
    int arg_index;
    static const char *opts[] = {
        "-to", "-utf8validate", "-smartquotes", 
        "-safe", "-width",
        NULL,
    };
    enum optflags {
        TCL_CMARK_TO, TCL_CMARK_VALIDATE_UTF8, TCL_CMARK_SMART,
        TCL_CMARK_SAFE, TCL_CMARK_WIDTH,
    };
    static const char *fmts[] = {
        "html", "text", "xml", 
        "latex", "commonmark", "man",
        NULL
    };
    enum fmtflags {
        TCL_CMARK_HTML, TCL_CMARK_TEXT, TCL_CMARK_XML, 
        TCL_CMARK_LATEX, TCL_CMARK_CMARK, TCL_CMARK_MAN,
    };
    enum fmtflags render_fmt = TCL_CMARK_HTML;
    int cmark_opts = CMARK_OPT_DEFAULT;
    int width = 0;;
    cmark_node *document = NULL;
    cmark_parser *parser = NULL;
    int nbytes, res = TCL_ERROR;
    char *cmark_text, *rendered;
    Tcl_Obj *o;

    if (objc < 2) {
	Tcl_WrongNumArgs(interp, 1, objv, "?options? COMMONMARKTEXT");
        return TCL_ERROR;
    }

    core_extensions_ensure_registered();
    
    for (arg_index = 1; arg_index < objc-1; ++arg_index) {
	int opt_index;
        int fmt_index;

	if (Tcl_GetIndexFromObj(interp, objv[arg_index], opts, "option", 0,
		&opt_index) != TCL_OK)
	    return TCL_ERROR;

	switch ((enum optflags) opt_index) {
        case TCL_CMARK_TO:
            if (++arg_index >= objc-1)
                goto missing_arg_error;
            if (Tcl_GetIndexFromObj(interp, objv[arg_index], fmts, "format", 0,
                                    &fmt_index) != TCL_OK)
                return TCL_ERROR;
            render_fmt = fmt_index;
            break;
        case TCL_CMARK_VALIDATE_UTF8:
            cmark_opts |= CMARK_OPT_VALIDATE_UTF8;
            break;
        case TCL_CMARK_SMART:
            cmark_opts |= CMARK_OPT_SMART;
            break;
        case TCL_CMARK_SAFE:
            cmark_opts |= CMARK_OPT_SAFE;
            break;
        case TCL_CMARK_WIDTH:
            if (++arg_index >= objc-1)
                goto missing_arg_error;
            res = Tcl_GetIntFromObj(interp, objv[arg_index], &width);
            if (res != TCL_OK)
                return res;
            if (width < 0)
                width = 0;
            break;
        }
    }

    cmark_text = Tcl_GetStringFromObj(objv[objc-1], &nbytes);

#ifdef TCL_CMARK_USE_ARENA
    parser = cmark_parser_new_with_mem(cmark_opts,
                                       cmark_get_arena_mem_allocator());
#else
    parser = cmark_parser_new(cmark_opts);
#endif
    if (parser == NULL)
        goto document_error;

    /* Load the standard extensions */
    {
        cmark_syntax_extension *ext = cmark_find_syntax_extension("strikethrough");
        if (ext == NULL) {
            Tcl_AppendResult(interp, "Error loading CommonMark extension ", "strikethrough", NULL);
            res = TCL_ERROR;
            goto vamoose;
        }
        cmark_parser_attach_syntax_extension(parser, ext);
    }
    
    cmark_parser_feed(parser, cmark_text, nbytes);

    document = cmark_parser_finish(parser);
    if (document == NULL)
        goto document_error;
    
    switch (render_fmt) {
        case TCL_CMARK_HTML: 
            rendered = cmark_render_html_with_mem(document, cmark_opts, 
                                                  NULL,
                                                  &TclCMark_Allocator);
            break;
        case TCL_CMARK_TEXT:
            rendered = cmark_render_plaintext_with_mem(document, cmark_opts, 
                                                       width, &TclCMark_Allocator);
            break;
        case TCL_CMARK_XML:
            rendered = cmark_render_xml_with_mem(document, cmark_opts, 
                                                 &TclCMark_Allocator);
            break;
        case TCL_CMARK_LATEX:
            rendered = cmark_render_latex_with_mem(document, cmark_opts, 
                                                   width, &TclCMark_Allocator);
            break;
        case TCL_CMARK_CMARK:
            rendered = cmark_render_commonmark_with_mem(document, cmark_opts, 
                                                        width,
                                                        &TclCMark_Allocator);
            break;
        case TCL_CMARK_MAN:
            rendered = cmark_render_man_with_mem(document, cmark_opts, 
                                                 width, &TclCMark_Allocator);
            break;
    }

    o = Tcl_NewObj();
    /*
     * Invalid string rep even if new Tcl_Obj because we do not want 
     * to rely on how it's fields are initialized 
     */
    Tcl_InvalidateStringRep(o);
    o->bytes = rendered;
    o->length = strlen(rendered);
    Tcl_SetObjResult(interp, o);
    res = TCL_OK;

vamoose:

#ifdef TCL_CMARK_USE_ARENA
    /* All memory is freed, no need to free parser and document */
    cmark_arena_reset();
#else
    if (parser)
        cmark_parser_free(parser);
    if (document)
        cmark_node_free(document);
#endif

    return res;

document_error:
    /* We do not really have a means to get error detail */
    Tcl_SetResult(interp, "Error parsing document.", TCL_STATIC);
    res = TCL_ERROR;
    goto vamoose;

missing_arg_error: /* arg_index-1 should point to offending option! */
    Tcl_AppendResult(interp, "No value specified for option ", Tcl_GetString(objv[arg_index-1]), ".", NULL);
    res = TCL_ERROR;
    goto vamoose;
    
}

/* Note the capitalization in Tclcmark_Init is as expected by Tcl loader */
int Cmark_Init(Tcl_Interp *interp)
{
#ifdef USE_TCL_STUBS
    if (Tcl_InitStubs(interp, "8.1", 0) == NULL) {
	return TCL_ERROR;
    }
#endif
    if (Tcl_PkgProvide(interp, PACKAGE_NAME, PACKAGE_VERSION) != TCL_OK) {
	return TCL_ERROR;
    }
    Tcl_CreateObjCommand(interp, "cmark::render", TclCMark_RenderObjCmd,
	    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);

    return TCL_OK;
}
