#include <iostream>
#include <fstream>
#include <sstream>
#include <clocale>
#include <string>
#include <vector>
#include <set>

std::vector<char> loadIndex(std::string const& fileName)
{
	std::ifstream fin;
	fin.open(fileName, std::ios::in | std::ios::binary);
	if(!fin)
		return std::vector<char>();

	// Получаем размер файла.
	fin.seekg(0, std::ios::end);
	std::streampos const size = fin.tellg();
	fin.seekg(0, std::ios::beg);

	std::vector<char> buf;
	buf.resize(size);
	fin.read(&buf[0], size);

	fin.close();
	return buf;
}

inline void makeUnique(std::vector<unsigned int>& docID)
{
	std::set<unsigned int> temp;
	temp.insert(docID.begin(), docID.end());
	docID.assign(temp.begin(), temp.end());
}

void appendDocID(void const* const data, std::vector<unsigned int>& docID)
{
	unsigned int const* const iter = reinterpret_cast<unsigned int const*>(data);
	// Не должно быть идентификатора термина равным нулю. Если это так,
	// то скорее всего это ошибка файла поэтому ничего не делаем.
	// Если количество документов нулевое, то это не ошибка, однако
	// в этом случае нам тоже делать нечего.
	if(iter[0] != 0xAAAAAAAA || iter[1] == 0 || iter[2] == 0)
		return;
	docID.insert(docID.end(), &iter[4], &iter[4] + iter[2]);
}

void getCompleteDocID(std::vector<char> const& data, std::vector<unsigned int>& docID)
{
	//docID.clear();
	if(data.empty())
		return;

	unsigned int const* iter = (unsigned int*)(&data[0] + 16);
	unsigned int const* const end = (unsigned int*)(&data[0] + data.size());

	while(true)
	{
		// Проверяем только то что нам нужно проверять:
		// метку блока и выход за границы памяти.
		if(iter[0] != 0xAAAAAAAA || iter >= end)
			break;
		appendDocID(iter, docID);
		iter += 4 + iter[2];
	}

	// Теперь удаляем все повторяющиеся идентификаторы.
	makeUnique(docID);
}

std::vector<unsigned int> lookupDocID(std::vector<char> const& data, unsigned int const termID)
{
	if(data.empty())
		return std::vector<unsigned int>();

	unsigned int const* iter = (unsigned int*)(&data[0] + 16);
	unsigned int const* const end = (unsigned int*)(&data[0] + data.size());

	while(true)
	{
		if(iter[0] != 0xAAAAAAAA || iter >= end)
			return std::vector<unsigned int>();
		/*if(iter[1] == termID)
			return getDocID(iter);*/
		iter += 4 + iter[2];
	}
}

int main(int argc, char** argv)
{
	std::setlocale(LC_CTYPE, "Russian");

	if(argc != 3)
	{
		std::cout << "IR4.exe index.dat запрос" << std::endl;
		return 1;
	}

	// Считываем весь индекс целиком в ОЗУ.
	std::vector<char> const rawIndex = loadIndex(argv[1]);
	std::vector<unsigned int> docID;
	getCompleteDocID(rawIndex, docID);

	// Получим поисковой запрос.
	//std::stringstream ss(argv[2]);

	std::cout << docID.size() << std::endl;
	std::system("pause");
	return 0;
}
