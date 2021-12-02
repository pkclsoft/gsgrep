# EZGrep - A grep like tool for the Apple IIGS 

Whilst by no means a complete implementation of the linux grep tool, this tool provides enough functionality to serve (hopefully) most purposes.

Written to compile under ORCA/C, and work in the ORCA/M or APW environments, the tool provides the following command line and options:

grep [-HhinR] pattern [file ...]

: -I	Perform case insensitive matching.  By default, grep is case sensitive.
: -H	Always print filename headers with output lines.
: -h	Never print filename headers (i.e. filenames) with output lines.
: -n	Each output line is preceded by its relative line number in the file, starting at line 1.  The line number counter is reset for each file processed.
: -R	Recursively search subdirectories listed.

***pattern*** follows the regular expression syntax as follows:

:   '.'        Dot, matches any character
:   '^'        Start anchor, matches beginning of string
:   '$'        End anchor, matches end of string
:   '*'        Asterisk, match zero or more (greedy)
:   '+'        Plus, match one or more (greedy)
:   '?'        Question, match zero or one (non-greedy)
:   '[abc]'    Character class, match if one of {'a', 'b', 'c'}
:   '[a-zA-Z]' Character ranges, the character set of the ranges { a-z | A-Z }
:   '\s'       Whitespace, \t \f \r \n \v and spaces
:   '\S'       Non-whitespace
:   '\w'       Alphanumeric, [a-zA-Z0-9_]
:   '\W'       Non-alphanumeric
:   '\d'       Digits, [0-9]
:   '\D'       Non-digits

If no file arguments are specified, the standard input is used.

## Acknowledgements
This tool borrows heavily, code from two other Github projects:

* tiny-regex-c at https://github.com/kokke/tiny-regex-c for which I have my own fork with an AppleIIGS branch at: https://github.com/pkclsoft/tiny-regex-c/tree/AppleIIGS

* parg at https://github.com/jibsen/parg, for which I have my own fork with an AppleIIGS branch at: https://github.com/pkclsoft/parg/tree/AppleIIGS

In addition, the original starting point of the mainline code of grep.c was sourced from another regex library on github: https://github.com/emulvaney/regex; I took the original code and updated to integrate the above two codebases, and to be suitable for the ORCA/M environment.
