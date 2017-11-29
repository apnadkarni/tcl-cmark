# NAME

**cmark** -- Tcl interface to CommonMark / Github Flavored Markdown

## SYNOPSIS

```
    package require cmark
    cmark::render ?options? TEXT
```

## DESCRIPTION

The `cmark` package provides a Tcl script level interface to the 
[cmark-gfm](https://github.com/github/cmark) library for parsing
and rendering [CommonMark](http://commonmark.org/) formatted text
which is a formalized specification of the commonly used Markdown syntax.
The library includes support for Github extensions which add several
features to the original specification. The extended syntax 
accepted by the library is described in the
[Github Flavored Markdown](https://github.github.com/gfm/)
specification. Here we refer to all these syntax variants collectively
as CommonMark.

The primary command exported from the package is the `render` command.
>
```
    cmark::render ?options? *TEXT*
```

The command parses `TEXT` which should be in CommonMark format and
returns it rendered in one of several different output formats.  Currently
supported output formats are HTML, Latex, Groff manpage, CommonMark
XML, CommonMark and plain text.

### Parsing
The table below summarizes the available options that affect parsing
of `TEXT`.

Option|Description
------|-----------
`-footnotes`|Enable parsing of footnotes.
`-ghprelang`|Uses Github style tags for code blocks that have a language attribute. This uses the `lang` attribute on the `<pre>` tag instead of a class on the `<code>` tag that wrap the block.
`-liberaltag`|Be liberal in interpretation of HTML tags.
`-smart`|Replaces straight quotes with curly quotes and double or triple hyphens with en dash and em dash respectively.
`-utf8validate`|Checks and replaces invalid UTF-8 character in `TEXT` with the U+FFFD replacement character.

### Rendering

The following options control the rendering of the input text.

Option|Description
------|-----------
`-hardbreaks`|Renders soft line breaks as hard line breaks.
`-nobreaks`|Renders soft line breaks as spaces.
`-safe`|Removes raw HTML by placeholder comments and unsafe links such as `javascript:`, `vbscript:` etc. by empty strings.
`-sourcepos`|Includes the `data-sourcepos` attribute on all block elements.
`-to` `FORMAT`|Specifies the output format. `FORMAT` must be one of `html`, `latex`, `man`, `xml`, `commonmark` or `text`.
`-width`|Specifies a max line width for wrapping. The default of `0` implies no wrapping. Only applies to CommonMark, Latex and man output formats.

Note that the HTML output format is a fragment and does not include any HTML 
header boilerplate.

### Enabling GFM extensions

By default, the command adheres to the [CommonMark](http://commonmark.org/)
specification. Various extensions specified in 
[Github Flavored Markdown](https://github.github.com/gfm/) (GFM) 
may be enabled through the options below.

Option|Description
------|-----------
`-gfm`|Enables all GFM extensions. Equivalent to specifying all options below.
`-autolink`|Enables [autolink](https://github.github.com/gfm/#autolinks-extension-) enhancements.
`-strikethrough`|Enables the [strikethrough](https://github.github.com/gfm/#strikethrough-extension-) extension for wrapping strikethrough text in tilde `~` characters.
`-table`|Enables the [table](https://github.github.com/gfm/#tables-extension-) for formatting tables.
`-tagfilter`|Enables the [tagfilter](https://github.github.com/gfm/#disallowed-raw-html-extension-) extension that disallows certain HTML tags.

## AUTHOR

Ashok P. Nadkarni
