#ifndef LOG_HEADER
#define LOG_HEADER

void open_log(char* log_path);

void close_log();

void write_log_format(_In_z_ _Printf_format_string_ const char * _Format, ...);

void write_log_and_print(_In_z_ _Printf_format_string_ const char * _Format, ...);

void write_log(const char* message);

#endif