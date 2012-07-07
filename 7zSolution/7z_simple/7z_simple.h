/* 7z_simple.h -- 7z interface
2012-07-04 : yedaoq : Public domain */

#ifndef __7Z_SIMPLE_H
#define __7Z_SIMPLE_H

#include "7z.h"
#include "7zFile.h"

EXTERN_C_BEGIN

int decompress_by_path(wchar_t const* archieve_path, wchar_t const* target_dir);
int decompress_by_handle(CSzFile archieve_file, wchar_t const* target_dir);
int decompress_by_buffer(void* archieve_buf, size_t archieve_size, wchar_t const* target_dir);
int decompress_by_stream(ISeekInStream* archieve_stream, wchar_t const* target_dir);

void PrintError(char *sz);

EXTERN_C_END

#endif
