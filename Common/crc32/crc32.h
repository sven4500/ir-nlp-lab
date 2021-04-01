#pragma once
#ifndef CRC32_H
#define CRC32_H

#include <stdlib.h> // ��� size_t.

// ���� uint8_t � uint32_t �������� ����� <cstdint> ������� � C++11.
// ����� ���������� ����� ������ ��������, ��� ��� ������ ���������
// � ��� ���. ������� ������������� ����������� Microsoft __int.
typedef unsigned __int8 uint8_t;
typedef unsigned __int32 uint32_t;

uint32_t crc32(uint32_t crc, const void *buf, size_t size);

#endif
