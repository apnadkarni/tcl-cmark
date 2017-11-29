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
#include <limits.h>
#include "tcl.h"
#include "cmark.h"
#include "core-extensions.h"

extern DLLEXPORT int Cmark_Init(Tcl_Interp * interp);

Tcl_Config tclcmark_config[] = {
    {"cmark,version", CMARK_VERSION_STRING},
    {NULL, NULL}
};

static void tclcmark_memory_panic()
{
    Tcl_Panic("Memory allocation request exceeds limit.");
}

/*
 * Memory allocator to use to allocate rendered result. We use Tcl's 
 * allocator so we can directly use allocated memory as interpreter result.
 * We need wrappers because parameter types differ (int v/s size_t).
 */
void *tclcmark_calloc(size_t nmem, size_t size)
{
    size_t nbytes;
    void *p;
    if (nmem > INT_MAX || size > INT_MAX)
        tclcmark_memory_panic();

    nbytes = nmem * size;       /* Cannot overflow because of above checks */
    if (nbytes > INT_MAX)
        tclcmark_memory_panic();

    p = ckalloc((int)nbytes);   /* Panics on failure to allocate */
    memset(p, 0, nbytes);
    return p;
}

void *tclcmark_realloc(void *p, size_t size)
{
    if (size > INT_MAX)
        tclcmark_memory_panic();
    return ckrealloc(p, (int) size);
}

void tclcmark_free(void *p)
{
    ckfree(p);
}

static cmark_mem tclcmark_allocator = {
    tclcmark_calloc, tclcmark_realloc, tclcmark_free,
};

/*
 * Load a CommonMark extension of the specified name.
 * Returns TCL_OK/TCL_ERROR with an appropriate error message stored
 * in the interp in the latter case.
 */
static int tclcmark_load_extension(
    Tcl_Interp *interp,
    cmark_parser *parser,
    const char *name)
{
    cmark_syntax_extension *ext = cmark_find_syntax_extension(name);
    if (ext == NULL) {
        Tcl_AppendResult(interp, "Error loading CommonMark extension ", 
                         name, NULL);
        return TCL_ERROR;
    }
    cmark_parser_attach_syntax_extension(parser, ext);
    return TCL_OK;
}

/*
 * render MARKDOWNTEXT
 *
 * Renders markdown text in the specified format, returning it as the
 * interp result.
 */

static int tclcmark_render_cmd(
    ClientData clientData,	/* Not used. */
    Tcl_Interp *interp,		/* Current interpreter */
    int objc,			/* Number of arguments */
    Tcl_Obj *const objv[]	/* Argument strings */
    )
{
    int arg_index;
    static const char *opts[] = {
        "-to", "-utf8validate", "-smart", 
        "-safe", "-width", "-footnotes",
        "-gfm", "-table", "-strikethrough",
        "-autolink", "-tagfilter",
        "-sourcepos", "-hardbreaks", "-nobreaks",
        "-liberaltag", "-ghprelang",
        NULL,
    };
    enum optflags {
        TCL_CMARK_TO, TCL_CMARK_VALIDATE_UTF8, TCL_CMARK_SMART,
        TCL_CMARK_SAFE, TCL_CMARK_WIDTH, TCL_CMARK_FOOTNOTES,
        TCL_CMARK_GFM, TCL_CMARK_TABLE, TCL_CMARK_STRIKETHROUGH,
        TCL_CMARK_AUTOLINK, TCL_CMARK_TAGFILTER,
        TCL_CMARK_SOURCEPOS, TCL_CMARK_HARDBREAKS, TCL_CMARK_NOBREAKS,
        TCL_CMARK_LIBERALTAG, TCL_CMARK_GHPRELANG,
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
    int width = 0;
    int gfm = 0;
    int table = 0;
    int strikethrough = 0;
    int autolink = 0;
    int tagfilter = 0;
    cmark_node *document = NULL;
    cmark_parser *parser = NULL;
    cmark_llist *syntax_extensions;
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
        case TCL_CMARK_FOOTNOTES:
            cmark_opts |= CMARK_OPT_FOOTNOTES;
            break;
        case TCL_CMARK_SOURCEPOS:
            cmark_opts |= CMARK_OPT_SOURCEPOS;
            break;
        case TCL_CMARK_HARDBREAKS:
            cmark_opts |= CMARK_OPT_HARDBREAKS;
            break;
        case TCL_CMARK_NOBREAKS:
            cmark_opts |= CMARK_OPT_NOBREAKS;
            break;
        case TCL_CMARK_LIBERALTAG:
            cmark_opts |= CMARK_OPT_LIBERAL_HTML_TAG;
            break;
        case TCL_CMARK_GHPRELANG:
            cmark_opts |= CMARK_OPT_GITHUB_PRE_LANG;
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
        case TCL_CMARK_GFM:           gfm = 1; break;
        case TCL_CMARK_TABLE:         table = 1; break;
        case TCL_CMARK_STRIKETHROUGH: strikethrough = 1; break;
        case TCL_CMARK_AUTOLINK:      autolink = 1; break;
        case TCL_CMARK_TAGFILTER:     tagfilter = 1; break;
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
    if (gfm || table) {
        res = tclcmark_load_extension(interp, parser, "table");
        if (res != TCL_OK)
            goto vamoose;
    }
    if (gfm || strikethrough) {
        res = tclcmark_load_extension(interp, parser, "strikethrough");
        if (res != TCL_OK)
            goto vamoose;
    }
    if (gfm || autolink) {
        res = tclcmark_load_extension(interp, parser, "autolink");
        if (res != TCL_OK)
            goto vamoose;
    }
    if (gfm || tagfilter) {
        res = tclcmark_load_extension(interp, parser, "tagfilter");
        if (res != TCL_OK)
            goto vamoose;
    }

    cmark_parser_feed(parser, cmark_text, nbytes);

    document = cmark_parser_finish(parser);
    if (document == NULL)
        goto document_error;
    
    switch (render_fmt) {
    case TCL_CMARK_HTML: 
        syntax_extensions = cmark_parser_get_syntax_extensions(parser);
        rendered = cmark_render_html_with_mem(document, cmark_opts, 
                                              syntax_extensions,
                                              &tclcmark_allocator);
        break;
    case TCL_CMARK_TEXT:
        rendered = cmark_render_plaintext_with_mem(document, cmark_opts, 
                                                   width, &tclcmark_allocator);
        break;
    case TCL_CMARK_XML:
        rendered = cmark_render_xml_with_mem(document, cmark_opts, 
                                             &tclcmark_allocator);
        break;
    case TCL_CMARK_LATEX:
        rendered = cmark_render_latex_with_mem(document, cmark_opts, 
                                               width, &tclcmark_allocator);
        break;
    case TCL_CMARK_CMARK:
        rendered = cmark_render_commonmark_with_mem(document, cmark_opts, 
                                                    width,
                                                    &tclcmark_allocator);
        break;
    case TCL_CMARK_MAN:
        rendered = cmark_render_man_with_mem(document, cmark_opts, 
                                             width, &tclcmark_allocator);
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

#if 0 /* Disabled because it relies on following internal structures */

#include <stdbool.h>
struct cmark_syntax_extension {
  cmark_match_block_func          last_block_matches;
  cmark_open_block_func           try_opening_block;
  cmark_match_inline_func         match_inline;
  cmark_inline_from_delim_func    insert_inline_from_delim;
  cmark_llist                   * special_inline_chars;
  char                          * name;
  void                          * priv;
  bool                            emphasis;
  cmark_free_func                 free_function;
  cmark_get_type_string_func      get_type_string_func;
  cmark_can_contain_func          can_contain_func;
  cmark_contains_inlines_func     contains_inlines_func;
  cmark_common_render_func        commonmark_render_func;
  cmark_common_render_func        plaintext_render_func;
  cmark_common_render_func        latex_render_func;
  cmark_common_render_func        man_render_func;
  cmark_html_render_func          html_render_func;
  cmark_html_filter_func          html_filter_func;
  cmark_postprocess_func          postprocess_func;
  cmark_opaque_free_func          opaque_free_func;
  cmark_commonmark_escape_func    commonmark_escape_func;
};
cmark_llist *cmark_list_syntax_extensions(cmark_mem *);
static int tclcmark_extensions_cmd(
    ClientData clientData,	/* Not used. */
    Tcl_Interp *interp,		/* Current interpreter */
    int objc,			/* Number of arguments */
    Tcl_Obj *const objv[]	/* Argument strings */
    )
{
  cmark_llist *syntax_extensions;
  cmark_llist *tmp;
  Tcl_Obj *oexts;
  cmark_mem *mem;

  core_extensions_ensure_registered();

  oexts = Tcl_NewListObj(0, NULL);
  mem = cmark_get_default_mem_allocator();

  syntax_extensions = cmark_list_syntax_extensions(mem);
  for (tmp = syntax_extensions; tmp; tmp=tmp->next) {
    struct cmark_syntax_extension *ext = (cmark_syntax_extension *) tmp->data;
    Tcl_ListObjAppendElement(interp, oexts, Tcl_NewStringObj(ext->name, -1));
  }

  cmark_llist_free(mem, syntax_extensions);
  Tcl_SetObjResult(interp, oexts);
  return TCL_OK;
}
#endif

/* Note the capitalization in Tclcmark_Init is as expected by Tcl loader */
int Cmark_Init(Tcl_Interp *interp)
{
#ifdef USE_TCL_STUBS
    if (Tcl_InitStubs(interp, "8.1", 0) == NULL) {
	return TCL_ERROR;
    }
#endif
    if (Tcl_PkgProvide(interp, PACKAGE_TCLNAME, PACKAGE_VERSION) != TCL_OK) {
	return TCL_ERROR;
    }
    Tcl_RegisterConfig(interp, PACKAGE_TCLNAME, tclcmark_config, "iso8859-1");
    Tcl_CreateObjCommand(interp, "cmark::render", tclcmark_render_cmd,
	    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
#if 0
    Tcl_CreateObjCommand(interp, "cmark::extensions", 
                         tclcmark_extensions_cmd,
                         (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
#endif
    return TCL_OK;
}
