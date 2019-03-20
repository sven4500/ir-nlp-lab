#pragma once
#ifndef MYINDEX_H
#define MYINDEX_H

namespace myind
{

	unsigned int const FileSignature = 0xAFAF;

#pragma pack(push, 4)

struct FileHeader
{
	unsigned short _sign;
	unsigned short _ver;
};

struct TermIndex
{
	unsigned int _sign;
	unsigned int _prev;
	unsigned int _next;
	unsigned int _termNum;
	unsigned int _termID;
	unsigned int _docIDCount;
	//unsigned __int32 _docID[];
};

#pragma pack(pop)

}

#endif
