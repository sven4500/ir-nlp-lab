#pragma once
#ifndef ZONERANGE_H
#define ZONERANGE_H

#include <fstream>
#include <vector>

// Функция зонного ранжирования. На вход принимает вектор идентификаторов
// документов удовлетворяющих поисковому запросу и вектор идентификаторов
// токенов из которых состоит поисковой запрос.
std::vector<std::pair<unsigned int, double>> zoneRange(std::vector<unsigned int> const& docID, std::vector<unsigned int> const& termID, std::ifstream& fin);

#endif
