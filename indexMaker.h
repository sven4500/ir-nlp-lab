#pragma once
#ifndef INDEXMAKER_H
#define INDEXMAKER_H

#include <string>
#include <tinyxml2.h>

// Абстрактный класс для создания индексов. Метод update принимает в качестве
// аргумента указатель на элемент XML страницы - фактически один документ.
// Предполагается циклический вызов метода update пока не закончатся статьи.
// Метод write создаёт индексный файл на диске и записывает данные в файл.
class IndexMaker
{
public:
    virtual void clear()=0;

    virtual bool update(tinyxml2::XMLElement const* element)=0;
    virtual bool write(std::string const& filename)=0;

protected:
    IndexMaker(){}
    ~IndexMaker(){}

};

#endif
