#include <cassert>
#include <unicode\unistr.h>
#include "textbeautifier.h"

inline void wipe_one(std::string& str, std::string const& seq)
{
	std::size_t pos = 0;
	while((pos = str.find(seq, pos)) != std::string::npos)
		str.replace(pos++, seq.length(), " ");
}

void wipe_all(std::string& str)
{
	static std::string const seq[] = {
		"\xE2\x80\x93", // EN DASH
		"\xE2\x80\x94", // EM DASH
		"\xE2\x88\x92", // MINUS SIGN
		"\xC2\xA0", // NON-BREAK SPACE
		"\xC2\xAB", // LEFT-POINTING DOUBLE ANGLE QUOTATION MARK
		"\xC2\xBB" // RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK
		/*"\xE2\x84\x96" // NUMERO SIGN*/
	};
	static unsigned int const size = sizeof(seq) / sizeof(std::string);
	for(unsigned int i = 0; i < size; ++i)
		wipe_one(str, seq[i]);
}

void to_lower_case(std::string& token)
{
    if(token.empty())
        return;
    icu::UnicodeString uni_token(token.c_str(), "UTF8");
    uni_token.toLower();
    // Тут нужно быть крайне осторожным. Метод toUTF8String добавляет
    // строку в конец, но не перетирвает уже имеющуюся информацию.
    token.clear();
    uni_token.toUTF8String(token);
}

std::size_t extract_token(std::string const& str, std::string& token, std::size_t pos)
{
    static std::string const delim(" ,.!?@$#&%_/*-+|<>(){}[]:;=`\\\"\'\x0A\x0D");
    std::size_t beg = str.find_first_not_of(delim, pos),
        end = str.find_first_of(delim, beg);
    if(beg == std::string::npos || end == std::string::npos)
        return std::string::npos;
    std::size_t const size = end - beg;
    token = str.substr(beg, size);
    assert(!token.empty());
    return end;
}
