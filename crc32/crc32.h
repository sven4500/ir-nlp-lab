#pragma once
#ifndef CRC32_H
#define CRC32_H

#include <stdlib.h> // Для size_t.

// Типы uint8_t и uint32_t доступны через <cstdint> начиная с C++11.
// Здесь используем более старый стандарт, так что такого заголовка
// у нас нет. Поэтому воспользуемся расширением Microsoft __int.
typedef unsigned __int8 uint8_t;
typedef unsigned __int32 uint32_t;

uint32_t crc32(uint32_t crc, const void *buf, size_t size);

#endif
