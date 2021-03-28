#pragma once
#ifndef TFIDFRANGE_H
#define TFIDFRANGE_H

#include <fstream>
#include <vector>

// ‘ункци€ производит оценку ранга списка документов из вектора docID. ¬ектор
// tokenID задаЄт идентификаторы токенов которые были использованы дл€
// составлени€ запроса. ‘ункци€ возвращает вектор той же длины что и вектор
// docID. Ёлементами вектора €вл€ютс€ пары значений <идентификатор документа;
// ранг TF-IDF>.
std::vector<std::pair<unsigned int, double>> TFIDFRange(std::vector<unsigned int> const& docID, std::vector<unsigned int> const& tokenID, std::ifstream& fin);

#endif
