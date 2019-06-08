#pragma once
#ifndef PARSER_H
#define PARSER_H

#include <fstream>
#include <vector>

std::vector<unsigned int> getDocID(std::ifstream& fin, unsigned int const termID);
std::vector<unsigned int> parse(std::ifstream& finInd, std::ifstream& finPosInd, std::ifstream& finTFIDF, char const* expr);

#endif
