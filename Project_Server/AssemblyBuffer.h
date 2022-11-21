#pragma once

struct AsemblyBuffer
{
	AsemblyBuffer(void);
	~AsemblyBuffer(void);

	void			Reset(void);
	BOOL			IsEmpty(void);
	BOOL			IsAssembling(void);

	size_t			Copy(const char* buff, size_t size);

	CHAR			m_Buffer[MAXIMUM_TRANSFER_SIZE] = { 0, };
	USHORT			m_curPos = 0;
	USHORT			m_endPos = 0;
};