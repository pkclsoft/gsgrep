; This makefile is for use with the mk tool which can be sourced at: https://github.com/pkclsoft/MK
;

re.a
	re.c re.h
		{
			assemble re.c keep=$
		}
		
parg.a
	parg.c parg.h
		{
			assemble parg.c keep=$
		}
		
grep.a
	grep.c
		{
			assemble grep.c keep=$
		}
		
grep
	grep.a re.a parg.a
		{
			link grep re parg keep=grep
		}
		
