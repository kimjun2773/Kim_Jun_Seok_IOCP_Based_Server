#include "stdafx.h" 
#include "AssemblyBuffer.h"

AsemblyBuffer::AsemblyBuffer(void)
{
	Reset();
}

AsemblyBuffer::~AsemblyBuffer(void)
{

}

void AsemblyBuffer::Reset(void)
{
	m_curPos = m_endPos = 0;
}

BOOL AsemblyBuffer::IsEmpty(void)
{
	return (0 == m_curPos && 0 == m_endPos);
}

BOOL AsemblyBuffer::IsAssembling(void)
{
	return (!IsEmpty() && m_curPos != m_endPos);
}

size_t AsemblyBuffer::Copy(const char* pPos, size_t size)
{
	if (IsEmpty())
	{
		memcpy(m_Buffer, pPos, size);
		m_curPos = size;
		m_endPos = *(reinterpret_cast<DWORD*>(m_Buffer));

		return size;
	}
	else if (IsAssembling())
	{
		size_t able2Cpy = min(size, m_endPos - m_curPos);
		memcpy(&m_Buffer[m_curPos], pPos, able2Cpy);
		m_curPos += able2Cpy;

		return able2Cpy;
	}
}