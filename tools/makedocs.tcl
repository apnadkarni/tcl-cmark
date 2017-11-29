package require cmark

set docdir [file join [file dirname [file dirname [info script]]] doc]

set fd [open [file join $docdir tclcmark.md]]
set mdtext [read $fd]
close $fd

set fd [open [file join $docdir tclcmark.html] w]
fconfigure $fd -encoding utf-8
puts $fd "<!DOCTYPE html>\n<html>\n<head>\n<title>Tcl cmark extension</title>\n</head>\n<body>"
puts $fd [cmark::render -to html -gfm -safe -smart $mdtext]
puts $fd "</body></html>"
close $fd

set fd [open [file join $docdir tclcmark.n] w]
fconfigure $fd -encoding utf-8
puts $fd [cmark::render -to man -gfm -safe -smart $mdtext]
close $fd
