#include <stdio.h>
//#include <stdlib.h>

#include "common.h"
#include "parse.h"
#include "print.h"
#include "bc.h"
#include "dbc.h"

/*static char* readfile(const char* path) {
	FILE* file;
	if (fopen_s(&file, path, "rb") || file == NULL) {
		fprintf(stderr, "Could not open file \"%s\".\n", path);
		exit(74);
	}

	fseek(file, 0L, SEEK_END);
	size_t filesize = ftell(file);
	rewind(file);

	char* buffer = (char*)malloc(filesize + 1);
	if (buffer == NULL) {
		fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
		exit(74);
	}

	size_t nbytes = fread(buffer, sizeof(char), filesize, file);
	if (nbytes < filesize) {
		fprintf(stderr, "Could not read file \"%s\".\n", path);
		exit(74);
	}

	buffer[nbytes] = '\0';

	fclose(file);
	return buffer;
}*/

int main() {
	char line[128];
	printf(">");
	while (gets_s(line, 128)) {
		if (!line[0]) break;
		K node = parse(line);

		// show parse tree
		K str = stringify(node);
		printf("%s\n", (const char*)PK(str));

		// compile & run
		K* c = compile(node, NULL);
		puts("bc:");
		B* b = (B*)c[2];
		dbc(b);
		puts("vm:");
		vmentry(c);

		printf(">");
	}
	return 0;
}
