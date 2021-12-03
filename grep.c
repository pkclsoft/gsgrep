/* 
A grep tool for the Apple IIGS
*/

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gsos.h>
#include <shell.h>
#include "re.h"
#include "parg.h"
#include "filetypes.h"

static void toLower(char *text) {
	int idx = 0;
	
	do {
		if (isalpha(text[idx])) {
			text[idx] = _tolower(text[idx]);
		}
		
		idx++;
	} while (text[idx] != 0x00);	
}

static int readline(char *buf, size_t len, FILE* fin) {
	if (fgets(buf, len, fin) == NULL) {
		return feof(fin) ? 0 : -1;
	}
	
	len = strlen(buf);
	
	if (buf[len-1] == '\n') {
		buf[--len] = '\0';
	}
	
	return 1;
}

enum Options {  /* bits */
	IgnoreCase = 1,
	ShowFilename = 2,
	ShowLineNumbers = 4,
	Recursive = 8,
	AllFiles = 16
};

static int grep(re_t regex, char *infile, int options) {
	char buf[BUFSIZ], bufCopy[BUFSIZ];
	int rc, matched = 0, matchLength = 0, lineNumber = 1;
	int standardInput = 0;
	
	FILE *fin = stdin;
	
	if (!infile) {
		standardInput = 1;
	} else if (infile && !strcmp(infile, "-")) {
		infile = "(standard input)";
		standardInput = 1;
	} else if(infile && (fin = fopen(infile, "r")) == NULL) {
		perror(infile);
		return -1;
	}
	
	while ((rc = readline(buf, sizeof buf, fin)) > 0) {
		// keep the original in case the case is changed.
		strcpy(bufCopy, buf);
		
		if ((options & IgnoreCase) != 0) {
			toLower(buf);
		}
		
		if(rc = re_matchp(regex, buf, &matchLength) >= 0) {
			matched = 1;
			
			if (((options & ShowFilename) != 0) && !standardInput) {
				if ((options & ShowLineNumbers) != 0) {
					printf("%s:%d:%s\n", infile, lineNumber, bufCopy);
				} else {
					printf("%s:%s\n", infile, bufCopy);
				}
			} else {
				printf("%s\n", bufCopy);
			}
		}
		
		lineNumber++;
		
		if (rc < 0) {
			break;
		}
	}
	
	
	if (rc < 0) {
		perror(infile ? infile : "(standard input)");
		matched = -1;
	}
	
	if (fin && fin != stdin && fclose(fin) == EOF) {
		perror(infile);
		return -1;
	}
	
	return matched;
}

typedef enum {
	Error = 0,
	Stopped = 1,
	Matched = 2,
	Unmatched = 3
} GrepResult;

GrepResult grepFile(re_t regex, char *thisFile, int flags) {
	ResultBuf255 filename;
	GSString255 inputName;
	
	Init_WildcardGSPB initwildparms;
	NextWildcardGSPB nextwildparms;
	StopGSPB stopparms;
	
	GrepResult result = Unmatched;
	int rc = 0;
	
	inputName.length = strlen(thisFile);
	strcpy(inputName.text, thisFile);
	
	initwildparms.pCount = 2;
	initwildparms.wFile = &inputName;
	initwildparms.flags = 0x8000; /* treat '?' as '=' */
	
	if ((flags & Recursive) != 0x00) {
		initwildparms.flags |= 0x2000;
	}
	
	InitWildcardGS(&initwildparms);
	
	filename.bufSize = 255;
	filename.bufString.length = 0;
	filename.bufString.text[0] = 0x00;
	
	nextwildparms.pCount = 3;
	nextwildparms.pathName = &filename;
	
	stopparms.pCount = 1;
	
	do {
		StopGS(&stopparms);
		
		if (stopparms.flag == 1) {
			result = Stopped;
		} else {
			NextWildcardGS(&nextwildparms);
			
			if (filename.bufString.length > 0) {
				// We need to look at the filetype of the file, and only check it
				// if it is a source or text file (unless -a) has been specified by
				// the user.
				//
				if (nextwildparms.fileType != ftDirectory) {
					if ((flags & AllFiles) ||
						((nextwildparms.fileType == ftText) ||
							(nextwildparms.fileType == ftHelp) ||
							(nextwildparms.fileType == ftSource)))
					{
						filename.bufString.text[filename.bufString.length] = 0x00;
						
						rc = grep(regex, filename.bufString.text, flags);
						
						if (rc >= 0) {
							result = Matched;
						} else if (rc < 0) {
							result = Error;
						}
					}
				}
			}
		}
	} while ((result >= Matched) && (filename.bufString.length > 0));
	
	return result;
}

int main(int argc, char *argv[]) {
	int matched = 0, errors = 0;
	int i, opt, flags = ShowFilename;
	struct parg_state ps;
	int optend;
	re_t regex;
	char *res;
	GrepResult grepResult = Unmatched;
	
	parg_init(&ps);
	
	// reorder the arguments for parg, so that options are first.
	//
	optend = parg_reorder(argc, argv, "inHh", NULL);
	
	// parse the options and arguments.
	//
	while ((errors == 0) && (opt = parg_getopt(&ps, optend, argv, "ainHhR")) != -1) {
		switch(opt) {
		case 'a': flags |= AllFiles;  	  
			break;
			
		case 'i': flags |= IgnoreCase;  	  
			break;
			
		case 'n': flags |= ShowLineNumbers;
			break;
			
		case 'H': flags |= ShowFilename;
			break;
			
		case 'h': flags &= (~ShowFilename);
			break;
			
		case 'R': flags |= Recursive;
			break;
			
		case 1:
			break;
			
		case '?':
			/*if (ps.optopt == 'o') {
			printf("option -o requires an argument\n");
			} else {*/
			errors = 1;
			//}
			break;
			
		default: 
			errors = 1;
		}
	}
	
	if ((errors != 0) || (i = ps.optind) >= argc) {
		fprintf(stderr, "usage: %s [-inHhR] (regex) [files...]\n", argv[0]);
		return 2;
	}
	
	// if we are ignoring case, then set the pattern to be all lower case.  the
	// same will be done as we read the file(s).
	//
	if ((flags & IgnoreCase) != 0) {
		toLower(argv[i]);
	}
	
	// compile the regular expression.
	regex = re_compile(argv[i++]);
	
	if (!regex) { 
		perror("failed to compile regular expression."); 
		return 2; 
	}
	
	if (i <= (argc - 0)) {
		do {
			grepResult = grepFile(regex, argv[i], flags);
			
			if (grepResult == Matched) {
				matched = 1;
			}
		} while ((grepResult >= Matched) && (++i < argc));
		
		if (grepResult < Matched) {
			errors = 1;
		}
	} else {
		int rc = grep(regex, NULL, flags);
		
		if (rc >= 0) {
			matched = 1;
		} else if (rc < 0) {
			errors = 1;
		}
	}
	
	return errors ? 2 : !matched;
}
