#ifndef UNISTD_H
#define UNISTD_H

/*
 * POSIX Compatibility Layer for Windows
 * 
 * This header provides Windows equivalents for common POSIX functions
 * that are used throughout the codebase. Only included on Windows.
 */

#ifdef _WIN32
    #include <io.h>
    #include <process.h>
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>

    // Sleep mapping (returns 0 to satisfy assert(usleep(...) == 0))
    #define usleep(us) (Sleep((us)/1000), 0)
    #define sleep(s) (Sleep((s)*1000), 0)

    // File access mapping
    #define access _access
    #define R_OK 4
    #define W_OK 2
    #define F_OK 0

    // Stat mapping
    #include <sys/types.h>
    #include <sys/stat.h>
    #define stat _stat
    #define fstat _fstat
    #define off_t _off_t

    // Standard I/O mapping
    #define fileno _fileno
    #define isatty _isatty

#endif // _WIN32

#endif // UNISTD_H
