

void main(){
	char *first="hello wrold , hello China";
	char *second = strstr(first,"wrold");

	printf("\"%s\" minus \"%s\" equls %d \n",first,second,second-first);
}
