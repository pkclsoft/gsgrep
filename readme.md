#grep - A grep like tool for the Apple IIGS 

Whilst by no means a complete implementation of the linux grep tool, this tool provides enough functionality to serve (hopefully) most purposes.

Written to compile under ORCA/C, and work in the ORCA/M or APW environments, the tool provides the following command line and options:

grep [-aHhinR] pattern [file ...]

* -a    Treat all files as ASCII text.  Normally grep will simply print ``Binary file ... matches`` if files are marked as not being textual.  Use of this option forces gsgrep to output lines matching the specified pattern.
* -i	Perform case insensitive matching.  By default, grep is case sensitive.
* -H	Always print filename headers with output lines.
* -h	Never print filename headers (i.e. filenames) with output lines.
* -n	Each output line is preceded by its relative line number in the file, starting at line 1.  The line number counter is reset for each file processed.
* -R	Recursively search subdirectories listed.

***pattern*** follows the regular expression syntax as follows:

*   '.'        Dot, matches any character
*   '^'        Start anchor, matches beginning of string
*   '$'        End anchor, matches end of string
*   '*'        Asterisk, match zero or more (greedy)
*   '+'        Plus, match one or more (greedy)
*   '?'        Question, match zero or one (non-greedy)
*   '[abc]'    Character class, match if one of {'a', 'b', 'c'}
*   '[a-zA-Z]' Character ranges, the character set of the ranges { a-z | A-Z }
*   '\s'       Whitespace, \t \f \r \n \v and spaces
*   '\S'       Non-whitespace
*   '\w'       Alphanumeric, [a-zA-Z0-9_]
*   '\W'       Non-alphanumeric
*   '\d'       Digits, [0-9]
*   '\D'       Non-digits

If no file arguments are specified, the standard input is used.

## Line Endings
The text and source files in this repository originally used CR line endings, as usual for Apple II text files, but they have been converted to use LF line endings because that is the format expected by Git. If you wish to move them to a real or emulated Apple II and build them there, you will need to convert them back to CR line endings.

If you wish, you can configure Git to perform line ending conversions as files are checked in and out of the Git repository. With this configuration, the files in your local working copy will contain CR line endings suitable for use on an Apple II. To set this up, perform the following steps in your local copy of the Git repository (these should be done when your working copy has no uncommitted changes):

1. Add the following lines at the end of the `.git/config` file:
```
[filter "crtext"]
	clean = LC_CTYPE=C tr \\\\r \\\\n
	smudge = LC_CTYPE=C tr \\\\n \\\\r
```

2. Run the following commands to convert the existing files in your working copy:
```
rm .git/index
git checkout HEAD -- .
```

Alternatively, you can keep the LF line endings in your working copy of the Git repository, but convert them when you copy the files to an Apple II. There are various tools to do this.  One option is `udl`, which is [available][udl] both as a IIGS shell utility and as C code that can be built and used on modern systems.

Another option, if you are using the [GSPlus emulator](https://apple2.gs/plus/) is to host your local repository in a directory that is visible on both your host computer, and the emulator via the excellent [Host FST](https://github.com/ksherlock/host-fst).

[udl]: http://ftp.gno.org/pub/apple2/gs.specific/gno/file.convert/udl.114.shk

## File Types
In addition to converting the line endings, you will also have to set the files to the appropriate file types before building ORCA/Modula-2 on a IIGS.

For each of the different groups of code, there is a`fixtypes` script (for use under the ORCA/M shell) that modifies the file and aux type of all source and build scripts, *apart from the fixtures script itself!*

So, once you have the files from the repository on your IIGS (or emulator), within the ORCA/M shell, execute the following command on each `fixtypes` script:

    filetype fixtypes src 6
    
## Acknowledgements
This tool borrows heavily, code from two other Github projects:

* tiny-regex-c at [github.com/kokke/tiny-regex-c](https://github.com/kokke/tiny-regex-c) for which I have my own fork with an AppleIIGS branch at: [github.com/pkclsoft/tiny-regex-c/tree/AppleIIGS](https://github.com/pkclsoft/tiny-regex-c/tree/AppleIIGS)

* parg at [github.com/jibsen/parg](https://github.com/jibsen/parg), for which I have my own fork with an AppleIIGS branch at: [github.com/pkclsoft/parg/tree/AppleIIGS](https://github.com/pkclsoft/parg/tree/AppleIIGS)

In addition, the original starting point of the mainline code of grep.c was sourced from another regex library on github: [github.com/emulvaney/regex](https://github.com/emulvaney/regex); I took the original code and updated to integrate the above two codebases, and to be suitable for the ORCA/M environment.

* Finally, the apple2_filetype.h code has been borrowed from [github.com/cc65/cc65](https://github.com/cc65/cc65) so that I don't reinvent the wheel.
