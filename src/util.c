#include "common.h"

#include <sys/types.h>
#include <sys/stat.h>

void log_err(char* msg) {
	if(msg == NULL)
		msg = "NULL ptr passed to log_err";
		
	fprintf(stderr, msg);
}

size_t filesize(const char* path) {
    struct stat buf;
    
    if(path == NULL) return (size_t) -1;
    
    stat(path, &buf);
    off_t size = buf.st_size;
    return size;
}




