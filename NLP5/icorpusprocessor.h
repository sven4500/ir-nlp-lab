#pragma once
#ifndef ICORPUSPROCESSOR_H
#define ICORPUSPROCESSOR_H

#include <tinyxml2.h>

// Бащовый класс для одного обработчика корпуса документов. Каждый ребёнок
// этого класса должен выполнять лишь одно простое действие над корпусом. Не
// чтоит использовать эту абстракцию для написания сложных обработчиков. Каждый
// ребёнок абстракции должен иметь хотя бы два переопределённых метода update и
// clear - один для накопления статистики, другой для сброса состояния. Метод
// dump переопределять не обязательно.
class ICorpusProcessor
{
public:
    virtual void update(tinyxml2::XMLElement const* elem)=0;
    virtual void clear()=0;

    // Производит запись данных в файл. Данными могут быть промежуточные, либо
    // итоговые результаты работы объекта.
    virtual bool dump(char const* filename)
    {
        return true;
    }

protected:
    ICorpusProcessor()
    {

    }

    virtual ~ICorpusProcessor()
    {
    
    }

};

#endif
