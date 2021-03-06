#include <stdio.h>
#include <windows.h>
#include <stdarg.h>
#include "Mutex.h"
#include "MutexConstants.h"
#include "Log.h"

static FILE* log_file;
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
//the file containe all the functions that responsable for the log file// 
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
void open_log(char* log_path)
{
	DWORD last_error = 0;
	if(log_file != NULL)
		return;

	last_error = GetLastError();
	log_file = fopen(log_path, "w+");
	if (log_file == NULL)
	{
		printf("Unable to open log file\n");
		exit(GetLastError());
	}
	SetLastError(last_error);
}

void close_log()
{
	if(log_file == NULL)
		return;
	fclose(log_file);
	log_file = NULL;
	close_mutex("LogFile");
}

void write_log(const char* message)
{
	if(log_file == NULL)
		return;

	lock_mutex(LOG_MUTEX);
	fputs(message, log_file);
	unlock_mutex(LOG_MUTEX);
}

void write_log_format(_In_z_ _Printf_format_string_ const char * _Format, ...)
{
	va_list args;
	if(log_file == NULL)
		return;
    va_start(args, _Format);
	
	lock_mutex(LOG_MUTEX);
    vfprintf(log_file, _Format, args);
	unlock_mutex(LOG_MUTEX);
	
    va_end(args);
}

void write_log_and_print(_In_z_ _Printf_format_string_ const char * _Format, ...)
{
	va_list args;
	if(log_file == NULL)
		return;
    va_start(args, _Format);
	
	lock_mutex(LOG_MUTEX);
    vfprintf(log_file, _Format, args);
	vprintf(_Format, args);
	unlock_mutex(LOG_MUTEX);
	
	va_end(args);
}