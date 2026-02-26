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



bool is_power_of_2(uint32_t x) {
    return x > 0 && !(x & (x-1));
}
uint64_t truncate_power_of_2(uint64_t x) {
	if(is_power_of_2(x) || x == 0) return x;
	return (1U << ilogb((double)x));
}

