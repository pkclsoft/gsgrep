/* 
  A grep tool for the Apple IIGS
*/

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re.h"
#include <parg.h>

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
	Recursive = 8
};

static int grep(re_t regex, char *infile, int options) {
	char buf[BUFSIZ], *captures[20];
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
		if(rc = re_matchp(regex, buf, &matchLength) >= 0) {
			matched = 1;
			
			if (((options & ShowFilename) != 0) && !standardInput) {
				if ((options & ShowLineNumbers) != 0) {
					printf("%s:%d:%s\n", infile, lineNumber, buf);
				} else {
					printf("%s:%s\n", infile, buf);
				}
			} else {
				printf("%s\n", buf);
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

int main(int argc, char *argv[]) {
	int matched = 0, errors = 0;
	int i, opt, rc, flags = ShowFilename;
	struct parg_state ps;
	int optend;
	re_t regex;
	
	parg_init(&ps);
	
	// reorder the arguments for parg, so that options are first.
	//
	optend = parg_reorder(argc, argv, "inHh", NULL);
	
	// parse the options and arguments.
	//
	while ((errors == 0) && (opt = parg_getopt(&ps, optend, argv, "inHhR")) != -1) {
		switch(opt) {
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

	// compile the regular expression.
	regex = re_compile(argv[i++]);
	
	if (!regex) { 
		perror("compile"); 
		return 2; 
	}
	
	do {
		rc = grep(regex, argv[i], flags);
		
		if (rc > 0) {
			matched = 1;
		} else if (rc < 0) {
			errors  = 1;
		}
	} while (++i < argc);
	
	return errors ? 2 : !matched;
}
