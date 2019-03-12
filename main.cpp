#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <vector>
#include <locale>
#include <unicode/unistr.h>

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

bool tokenize(std::ifstream& fin, std::set<std::string>& tokens)
{
	//std::locale loc;
	fin.seekg(0, std::ios::beg);

	std::string const delim(" ,.!?@$#&%_/*-+|<>(){}[]:;=`\\\"\'");
	std::string line;

	while(!getline(fin, line).eof())
	{
		// Исключаем дополнительные символы UTF-8.
		wipe_all(line);

		std::size_t pos = 0;
		std::size_t end = 0;

		while((end = line.find_first_of(delim, pos)) != std::string::npos)
		{
			std::size_t const size = end - pos;
			if(size > 1)
			{
				// Здесь всё равно могут затесаться знаки препинания и
				// другие символы из UTF8. Проблема в том что эти знаки
				// кодируются уде 2, 3 или 4 байтами. Для их отделения
				// нужно использовать уже немного другой метод.
				std::string const token(line.substr(pos, size));
				
				icu::UnicodeString uniToken(token.c_str(), "UTF8");
				uniToken.toLower();

				std::string finalToken;
				uniToken.toUTF8String(finalToken);

				//unicodeToken.toUTF8String(token);
				//std::transform(token.begin(), token.end(), token.begin(), tolower);
				if(!isdigit((unsigned char)finalToken[0]))
					tokens.insert(finalToken);
			}
			pos = end + 1;
		}
	}

	return !tokens.empty();
}

bool save(std::ofstream& fout, std::set<std::string> const& tokens)
{
	fout.seekp(0, std::ios::end);
	std::set<std::string>::const_iterator iter = tokens.cbegin();
	std::set<std::string>::const_iterator const end = tokens.cend();
	while(iter != end)
	{
		fout << *iter << std::endl;
		++iter;
	}
	return true;
}

int main(int argc, char** argv)
{
	setlocale(LC_ALL, "Russian");

	if(argc != 3)
	{
		std::cout << "NLP1.exe [входной файл] [выходной файл]" << std::endl;
		return -1;
	}

	std::set<std::string> tokens;

	{
		std::ifstream fin;
		fin.open(argv[1], std::ios::in);
		if(fin)
		{
			std::cout << "Обрабатываю файл: " << argv[1] << std::endl;
			tokenize(fin, tokens);
			fin.close();
		}
		else
		{
			std::cout << "Не могу открыть файл для чтения: " << argv[1] << std::endl;
		}
	}

	{
		std::ofstream fout;
		fout.open(argv[2], std::ios::out | std::ios::app);
        fout << std::endl << "##" << argv[1] << std::endl;

		if(fout)
		{
			save(fout, tokens);
			fout.close();
		}
		else
		{
			std::cout << "Не могу открыть файл для записи: " << argv[2] << std::endl;
		}
	}

	return 0;
}
