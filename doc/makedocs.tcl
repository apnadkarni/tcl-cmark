package require Tcl 9
set ver [package require cmark]
set docDir [file dirname [info script]]
set md [readFile [file join $docDir tclcmark.md]]
writeFile [file join $docDir tclcmark.html] [cmark::render $md]
writeFile [file join $docDir tclcmark.n] \
    [string cat \
         {'\"} \n \
         {'\" Copyright (c) 2017-2025 Ashok P. Nadkarni} \n \
         {'\"} \n \
         ".TH tclcmark n tclcmark $ver \"Tcl interface to Github-flavored Markdown\"" \n \
         {'\" See file LICENSE in source for licensing terms.} \n \
         [cmark::render -to man $md]]
