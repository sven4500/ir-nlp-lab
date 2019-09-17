#pragma once
#ifndef TEXTBEAUTIFIER_H
#define TEXTBEAUTIFIER_H

#include <string>

// Функция меняет на пробелы символы UTF-8 типа EN_DASH, MINUS_SIGN и пр.
void wipe_all(std::string& str);

// Функция извлекает ровно один токен заключённый между двумя ограничителями
// указанными в строке delim. Обновляет значение pos на конец токена.
std::size_t extract_token(std::string const& str, std::string& token, std::size_t pos);

void to_lower_case(std::string& token);

// isalnum зависит от локальной реализации поэтому используем нашц реализацию.
inline bool is_alpha_numeric(char ch)
{
    return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9');
}

#endif
