set ver [package require cmark]

set docdir [file join [file dirname [file dirname [file normalize [info script]]]] doc]

set fd [open [file join $docdir tclcmark.md]]
set mdtext [read $fd]
close $fd

set fd [open [file join $docdir tclcmark.html] w]
fconfigure $fd -encoding utf-8
puts $fd "<!DOCTYPE html>\n<html>\n<head>\n<title>Tcl cmark extension</title>\n</head>\n<body>"
puts $fd [cmark::render -to html -gfm -smart $mdtext]
puts $fd "</body></html>"
close $fd

set fd [open [file join $docdir tclcmark.n] w]
fconfigure $fd -encoding utf-8
puts $fd {'\"}
puts $fd {'\" Copyright (c) 2017-2025 Ashok P. Nadkarni}
puts $fd {'\"}
puts $fd {'\" See file LICENSE in source for licensing terms.}
puts $fd ".TH tclcmark n tclcmark $ver \"Tcl Github-flavored Markdown processor\""
puts $fd [cmark::render -to man -gfm -smart $mdtext]
close $fd
