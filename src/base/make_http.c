#include "../header.h"
#include "../base/make_http.h"

char *http_prepare_empty_request(int SIZE)
{
	char *str = (char*)malloc(sizeof(char)*SIZE);
	memset(str,'\0',SIZE);
	printf("%s\n",str);

	return str;
}

int http_find_next_place(char *s)
{
	int i=0;
	while(s[i]!='\0')
		i++;

	return i;
}

void http_set_start_line(char *s,char *method, char *target, char *httpver)
{
	int i = http_find_next_place(s);
	strcat(s,method);
	strcat(s," /");
	strcat(s,target);
	strcat(s," HTTP/");
	strcat(s,httpver);
	strcar(s,"\n");
}

void http_
