#include <iostream> // cout
#include <clocale> // setlocale
#include <tinyxml2.h>
#include "icorpusprocessor.h"
#include "collector.h"
#include "bigramcollector.h"
using namespace tinyxml2;

int const ArgCorpSrc = 1;
int const ArgFreq = 2;
int const ArgMonoOut = 3;
int const ArgBiOut = 4;
int const ArgStudOut = 5;
int const ArgMleOut = 6;

void processCorpus(XMLDocument* doc, ICorpusProcessor* proc)
{
    unsigned int pages = 0;

    XMLElement const* root = doc->FirstChildElement();
    XMLElement const* pageElem = (root) ? root->FirstChildElement() : 0;

    while(pageElem != 0)
    {
        if(pages % 1500 == 0)
            std::cout << "\rДокументов обработано " << pages;

        proc->update(pageElem);
        pageElem = pageElem->NextSiblingElement();
        ++pages;
    }

    std::cout << std::endl;
}

int main(int argc, char** argv)
{
    std::setlocale(LC_CTYPE, "Russian");
    if(argc != 7)
    {
        std::cout << "NLP5.exe _In_Corpus.xml _In_Freq _Out_Monograms.txt _Out_Bigrams _Out_BigramsStud _Out_BigramsMle" << std::endl;
        return -1;
    }

    std::cout << "Пытаюсь открыть корпус документов..." << std::endl;

    XMLDocument doc;
    if(doc.LoadFile(argv[ArgCorpSrc]) != XML_SUCCESS)
    {
        std::cout << "Не могу открыть корпус документов!" << std::endl;
        return -1;
    }

    // Получаем количество часто используемых токенов.
    int const freq = atoi(argv[ArgFreq]);
    std::cout << "Количество часто используемых токенов для удаления: " << freq << std::endl;

    Collector monograms;
    processCorpus(&doc, &monograms);
    monograms.dump(argv[ArgMonoOut]);

    BigramCollector bigrams;
    bigrams.rememberMostFrequent(monograms.mostFrequent(freq));
    processCorpus(&doc, &bigrams);
    bigrams.dump(argv[ArgBiOut]);

    return 0;
}
