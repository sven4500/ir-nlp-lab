#pragma once
#ifndef PARSER_H
#define PARSER_H

#include <fstream>
#include <vector>

extern unsigned int skip;
extern unsigned int skipCount;

std::vector<unsigned int> parse(char const* expr, std::ifstream& finInd, std::ifstream& finPosInd, std::ifstream& finTFIDF, std::ifstream& finZoneInd);

#endif
