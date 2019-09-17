#include <iostream> // cout
#include <clocale> // setlocale
#include <tinyxml2.h>
#include "icorpusprocessor.h"
#include "monogramcollector.h"
using namespace tinyxml2;

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
    if(argc != 5)
    {
        std::cout << "NLP5.exe _In_Corpus.xml _Out_Colloc.txt _In_MostFreqWordCount _In_CollocCount" << std::endl;
        return -1;
    }

    std::cout << "Пытаюсь открыть корпус документов..." << std::endl;

    XMLDocument doc;
    if(doc.LoadFile(argv[1]) != XML_SUCCESS)
    {
        std::cout << "Не могу открыть корпус документов!" << std::endl;
        return -1;
    }

    MonogramCollector monogramCollector;
    processCorpus(&doc, &monogramCollector);
    //monogramCollector.remove(30);
    monogramCollector.dump("monograms.txt");

    return 0;
}
