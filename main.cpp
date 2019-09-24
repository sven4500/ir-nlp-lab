#include <iostream> // cout
#include <clocale> // setlocale
#include <tinyxml2.h>
#include "icorpusprocessor.h"
#include "collector.h"
#include "bigramcollector.h"
#include "studproc.h"
#include "mleproc.h"
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
        if(pages % 1000 == 0)
            std::cout << "\r���������� ���������� " << pages;

        proc->update(pageElem);
        pageElem = pageElem->NextSiblingElement();
        ++pages;

        if(pages == 30000)
            break;
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

    std::cout << "������� ������� ������ ����������..." << std::endl;

    XMLDocument doc;
    if(doc.LoadFile(argv[ArgCorpSrc]) != XML_SUCCESS)
    {
        std::cout << "�� ���� ������� ������ ����������!" << std::endl;
        return -1;
    }

    // �������� ���������� ����� ������������ �������.
    int const freq = atoi(argv[ArgFreq]);
    std::cout << "���������� ����� ������������ ������� ��� ��������: " << freq << std::endl;

    Collector<unsigned int> monograms;
    processCorpus(&doc, &monograms);
    monograms.dump(argv[ArgMonoOut]);

    std::cout << "����� ���������������� �������: " << monograms.totalCount() << std::endl
        << "�� ��� ����������: " << monograms.count() << std::endl;

    BigramCollector<unsigned int> bigrams;
    bigrams.rememberMostFrequent(monograms.mostFrequent(freq));
    processCorpus(&doc, &bigrams);
    bigrams.dump(argv[ArgBiOut]);

    std::cout << "����� ���������������� �������: " << bigrams.totalCount() << std::endl
        << "�� ��� ���������: " << bigrams.count() << std::endl;

    StudProc<float> stud;
    stud.update(monograms, bigrams);
    stud.dump(argv[ArgStudOut]);
    stud.clear();

    std::cout << "����� ������� ���������� (t-�������� ���������): " << stud.count() << std::endl;

    MLEProc<float> mle;
    mle.update(monograms, bigrams);
    mle.dump(argv[ArgMleOut]);
    mle.clear();

    std::cout << "����� ������� ���������� (���): " << mle.count() << std::endl;

    return 0;
}
