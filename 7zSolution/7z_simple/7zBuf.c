/* 7zBuf.c -- Byte Buffer
2008-03-28
Igor Pavlov
Public domain */

#include "7zBuf.h"

#ifndef UNDER_CE
#include <errno.h>
#endif

void Buf_Init(CBuf *p)
{
  p->data = 0;
  p->size = 0;
}

int Buf_Create(CBuf *p, size_t size, ISzAlloc *alloc)
{
  p->size = 0;
  if (size == 0)
  {
    p->data = 0;
    return 1;
  }
  p->data = (Byte *)alloc->Alloc(alloc, size);
  if (p->data != 0)
  {
    p->size = size;
    return 1;
  }
  return 0;
}

void Buf_Free(CBuf *p, ISzAlloc *alloc)
{
  alloc->Free(alloc, p->data);
  p->data = 0;
  p->size = 0;
}

static SRes BufInStream_Read(void *pp, void *buf, size_t *size)
{
	//TRACE1("read buf - bytes : %u\n", *size);
	CBufInStream *p = (CBufInStream *)pp;
	if(p->buf.pos >= p->buf.size)
	{
		SetLastError(ERROR_HANDLE_EOF);
		return SZ_ERROR_READ;
	}

	*size = min(*size, p->buf.size - p->buf.pos);
	memcpy(buf, p->buf.data + p->buf.pos, *size);
	p->buf.pos += *size;
	return SZ_OK;
}

static SRes BufInStream_Seek(void *pp, Int64 *pos, ESzSeek origin)
{
	//TRACE1("seek to %d\n", (int)*pos);
	CBufInStream *p = (CBufInStream *)pp;
	Int64 target_pos;

	switch(origin)
	{
	case SZ_SEEK_CUR:
		target_pos = p->buf.pos + *pos;
		break;
	case SZ_SEEK_END:
		target_pos = p->buf.size;
		break;
	case SZ_SEEK_SET:
		target_pos = *pos;
		break;
	}

	if(target_pos < 0)
	{
		return SZ_ERROR_PARAM;
	}

	*pos = p->buf.pos = target_pos;
	return SZ_OK;
}

void BufInStream_CreateVTable( CBufInStream *p )
{
	p->s.Read = BufInStream_Read;
	p->s.Seek = BufInStream_Seek;
}
