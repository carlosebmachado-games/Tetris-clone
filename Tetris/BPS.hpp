/*
 * MIT License
 *
 * Copyright (c) 2020 Carlos Eduardo de Borba Machado
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef BPS_H
#define BPS_H

#include <string>
#include <vector>
#include <fstream>
#include <ctype.h>

#include "util.hpp"

namespace BPS
{
    // CONSTANTES
    const char* TK_SECTION = "<section>";
    const char* TK_DATA = "<data>";
    const char* TK_NAME = "<name>";
    const char* TK_CLOSE = "</>";

    const char SY_HASH = '#';
    const char SY_OPEN_BRACKETS = '[';
    const char SY_CLOSE_BRACKETS = ']';
    const char SY_LESS = '<';
    const char SY_GREATER = '>';
    const char SY_COMMA = ',';
    const char SY_COLON = ':';
    const char SY_DOT = '.';
    const char SY_MINUS = '-';
    const char SY_SPACE = ' ';
    const char SY_BAR = '/';
    const char SY_BAR_INV = '\\';
    const char SY_QUOTE = '\'';
    const char SY_DQUOTE = '\"';
    const char SY_TAB = '\t';
    const char SY_RETURN = '\r';
    const char SY_NEWLINE = '\n';

    const char* FILE_HEADER = "# BPS File";
    const char* FILE_TAB = "    ";

    const char* FILENAME_EXTENSION = ".bps";

    const char* TOKENS[4] =
    {
        TK_SECTION,
        TK_DATA,
        TK_NAME,
        TK_CLOSE
    };

    const char* OPEN_TOKENS[3] =
    {
        TK_SECTION,
        TK_DATA,
        TK_NAME
    };

    const char* CLOSE_TOKENS[1] =
    {
        TK_CLOSE
    };

    // DECLARAÇÃO
    class Data
    {
    private:
        std::string key;
    public:
        Data(std::string);
        virtual ~Data();
        std::string getKey();
        void setKey(std::string);
    };

    class StringData : public Data
    {
    private:
        std::string value;
    public:
        StringData(std::string, std::string);
        ~StringData();
        std::string getValue();
        void setValue(std::string);
    };

    class CharData : public Data
    {
    private:
        char value;
    public:
        CharData(std::string, char);
        ~CharData();
        char getValue();
        void setValue(char);
    };

    class BoolData : public Data
    {
    private:
        bool value;
    public:
        BoolData(std::string, bool);
        ~BoolData();
        bool getValue();
        void setValue(bool);
    };

    class IntData : public Data
    {
    private:
        int value;
    public:
        IntData(std::string, int);
        ~IntData();
        int getValue();
        void setValue(int);
    };

    class DoubleData : public Data
    {
    private:
        double value;
    public:
        DoubleData(std::string, double);
        ~DoubleData();
        double getValue();
        void setValue(double);
    };

    class ArrayData : public Data
    {
    private:
        std::vector<Data*> value;
    public:
        ArrayData(std::string, std::vector<Data*>);
        ~ArrayData();
        std::vector<Data*> getValue();
        void setValue(std::vector<Data*>);
    };

    class Section
    {
    private:
        std::string name;
        std::vector<Data*> data;
    public:
        Section(std::string, std::vector<Data*>);
        Section(std::string);
        ~Section();
        bool add(Data*);
        void removeAll();
        bool remove(std::string);
        std::vector<Data*> findAll();
        Data* find(std::string);
        bool exists(std::string);
        std::string getName();
        void setName(std::string);
    };

    class File
    {
    private:
        std::vector<Section*> sections;
    public:
        File(std::vector<Section*>);
        File();
        ~File();
        bool add(Section*);
        void removeAll();
        bool remove(std::string);
        std::vector<Section*> findAll();
        Section* find(std::string);
        bool exists(std::string);
    };

    std::string normalizePath(std::string);
    std::string removeComments(std::string);

    File* read(std::string);
    void write(File*, std::string);

    // token
    bool isToken(std::string);
    bool isOpenToken(std::string);
    bool isCloseToken(std::string);

    // parser
    std::vector<std::string> lexer(std::string);

    bool isString(std::string);
    bool isChar(std::string);
    bool isBool(std::string);
    bool isNumeric(std::string);
    bool isInt(std::string);
    bool isDouble(std::string);
    bool isArray(std::string);

    std::string parseString(std::string);
    char parseChar(std::string);
    bool parseBool(std::string);
    int parseInt(std::string);
    double parseDouble(std::string);
    template <typename T>
    Data* parseData(std::string, std::string);

    template <typename T>
    std::vector<Section*> parser(std::vector<std::string>);

    template <typename T>
    File* compile(std::string);

    // IMPLEMENTAÇÃO

    // abstract data
    Data::Data(std::string key)
    {
        this->key = key;
    }

    Data::~Data()
    {
        key.~basic_string();
    }

    std::string Data::getKey()
    {
        return key;
    }

    void Data::setKey(std::string key)
    {
        this->key = key;
    }

    // string data
    StringData::StringData(std::string key, std::string value) : Data(key)
    {
        this->value = value;
    }

    StringData::~StringData()
    {
        value.~basic_string();
    }

    std::string StringData::getValue()
    {
        return value;
    }

    void StringData::setValue(std::string value)
    {
        this->value = value;
    }

    // char data
    CharData::CharData(std::string key, char value) : Data(key)
    {
        this->value = value;
    }

    CharData::~CharData() {}

    char CharData::getValue()
    {
        return value;
    }

    void CharData::setValue(char value)
    {
        this->value = value;
    }

    // bool data
    BoolData::BoolData(std::string key, bool value) : Data(key)
    {
        this->value = value;
    }

    BoolData::~BoolData()
    {
    }

    bool BoolData::getValue()
    {
        return value;
    }

    void BoolData::setValue(bool value)
    {
        this->value = value;
    }

    // int data
    IntData::IntData(std::string key, int value) : Data(key)
    {
        this->value = value;
    }

    IntData::~IntData()
    {
    }

    int IntData::getValue()
    {
        return value;
    }

    void IntData::setValue(int value)
    {
        this->value = value;
    }

    // double data
    DoubleData::DoubleData(std::string key, double value) : Data(key)
    {
        this->value = value;
    }

    DoubleData::~DoubleData()
    {
    }

    double DoubleData::getValue()
    {
        return value;
    }

    void DoubleData::setValue(double value)
    {
        this->value = value;
    }

    // double data
    ArrayData::ArrayData(std::string key, std::vector<Data*> value) : Data(key)
    {
        this->value = value;
    }

    ArrayData::~ArrayData()
    {
    }

    std::vector<Data*> ArrayData::getValue()
    {
        return value;
    }

    void ArrayData::setValue(std::vector<Data*> value)
    {
        this->value = value;
    }

    // section
    Section::Section(std::string name, std::vector<Data*> data)
    {
        this->name = name;
        this->data = data;
    }

    Section::Section(std::string name)
    {
        this->name = name;
        data = std::vector<Data*>();
    }

    Section::~Section()
    {
        name.~basic_string();
        data.~vector();
    }

    bool Section::add(Data* data)
    {
        if (!exists(data->getKey()))
        {
            this->data.push_back(data);
            return true;
        }
        return false;
    }

    void Section::removeAll()
    {
        data.clear();
    }

    bool Section::remove(std::string key)
    {
        for (size_t i = 0; i < data.size(); i++)
        {
            if (data[i]->getKey() == key)
            {
                data.erase(data.begin() + i);
                return true;
            }
        }
        return false;
    }

    std::vector<Data*> Section::findAll()
    {
        return data;
    }

    Data* Section::find(std::string key)
    {
        for (auto d : data)
        {
            if (d->getKey() == key)
            {
                return d;
            }
        }
        return nullptr;
    }

    bool Section::exists(std::string key)
    {
        return find(key) != nullptr;
    }

    std::string Section::getName()
    {
        return name;
    }

    void Section::setName(std::string name)
    {
        this->name = name;
    }

    File::File(std::vector<Section*> sections)
    {
        this->sections = sections;
    }

    File::File()
    {
        sections = std::vector<Section*>();
    }

    File::~File()
    {
        sections.~vector();
    }

    bool File::add(Section* section)
    {
        if (!exists(section->getName()))
        {
            this->sections.push_back(section);
            return true;
        }
        return false;
    }

    void File::removeAll()
    {
        sections.clear();
    }

    bool File::remove(std::string key)
    {
        for (size_t i = 0; i < sections.size(); i++)
        {
            if (sections[i]->getName() == key)
            {
                sections.erase(sections.begin() + i);
                return true;
            }
        }
        return false;
    }

    std::vector<Section*> File::findAll()
    {
        return sections;
    }

    Section* File::find(std::string name)
    {
        for (auto s : sections)
        {
            if (s->getName() == name)
            {
                return s;
            }
        }
        return nullptr;
    }

    bool File::exists(std::string name)
    {
        return find(name) != nullptr;
    }

    std::string normalizePath(std::string path)
    {
        auto length = path.size();
        if (length > 4)
        {
            if (path.substr(length - 4, 4) != FILENAME_EXTENSION)
            {
                return path + FILENAME_EXTENSION;
            }
            else
            {
                return path;
            }
        }
        else if (path == FILENAME_EXTENSION)
        {
            return path;
        }
        else
        {
            return path + FILENAME_EXTENSION;
        }
    }

    std::string removeComments(std::string str)
    {
        auto r = split(str, SY_COLON);
        r[0] = trim(r[0]);
        return r[0];
    }

    // token
    bool isToken(std::string str)
    {
        for (std::string token : TOKENS)
        {
            if (token == str)
            {
                return true;
            }
        }
        return false;
    }

    bool isOpenToken(std::string str)
    {
        for (std::string token : OPEN_TOKENS)
        {
            if (token == str)
            {
                return true;
            }
        }
        return false;
    }

    bool isCloseToken(std::string str)
    {
        for (std::string token : CLOSE_TOKENS)
        {
            if (token == str)
            {
                return true;
            }
        }
        return false;
    }

    // parser
    std::vector<std::string> lexer(std::string data)
    {
        auto tokens = std::vector<std::string>();

        std::string cleanString = "";
        for (size_t i = 0; i < data.length(); i++)
        {
            if (data[i] == SY_DQUOTE)
            {
                cleanString += data[i];
                i++;
                while (data[i] != SY_DQUOTE && data[i - 1] != SY_BAR_INV)
                {
                    cleanString += data[i];
                    i++;
                }
            }
            if (data[i] == SY_HASH)
            {
                while (data[i] != SY_NEWLINE)
                {
                    i++;
                }
            }
            if (data[i] == SY_RETURN || data[i] == SY_NEWLINE ||
                data[i] == SY_SPACE || data[i] == SY_TAB)
            {
                continue;
            }
            cleanString += data[i];
        }

        std::string curWord = "";
        bool hasSomething = false;
        for (size_t i = 0; i < cleanString.length(); i++)
        {
            if (cleanString[i] == SY_LESS && !hasSomething)
            {
                hasSomething = true;
            }
            if (hasSomething)
            {
                if (cleanString[i] == SY_COLON)
                {
                    tokens.push_back(curWord);
                    tokens.push_back(std::string(1, cleanString[i]));
                    curWord = "";
                    continue;
                }
                if (cleanString[i] == SY_LESS && cleanString[i + 1] == SY_BAR)
                {
                    if (curWord != "")
                    {
                        tokens.push_back(curWord);
                    }
                    curWord = std::string(1, cleanString[i]);
                    continue;
                }
                curWord += cleanString[i];
                if (isToken(curWord))
                {
                    tokens.push_back(curWord);
                    if (!isOpenToken(curWord))
                    {
                        hasSomething = false;
                    }
                    curWord = "";
                }
            }
        }
        return tokens;
    }

    bool isString(std::string value)
    {
        return value[0] == SY_DQUOTE;
    }

    bool isChar(std::string value)
    {
        return value[0] == SY_QUOTE && value.length() == 3;
    }

    bool isBool(std::string value)
    {
        return value == "true" || value == "false";
    }

    bool isNumeric(std::string value)
    {
        return isdigit(value[0]) || value[0] == SY_MINUS;
    }

    bool isInt(std::string value)
    {
        if (isNumeric(value))
        {
            for (char c : value)
            {
                if (c == SY_DOT)
                {
                    return false;
                }
            }
            return true;
        }
        return false;
    }

    bool isDouble(std::string value)
    {
        if (isNumeric(value))
        {
            for (char c : value)
            {
                if (c == SY_DOT)
                {
                    return true;
                }
            }
        }
        return false;
    }

    bool isArray(std::string value)
    {
        return value[0] == SY_OPEN_BRACKETS;
    }

    std::string parseString(std::string value)
    {
        return value.substr(1, value.length() - 2);
    }

    char parseChar(std::string value)
    {
        std::string nv = value.substr(1, 1);
        const char* cvpc = nv.c_str();
        char* cvp = (char*)cvpc;
        char cv = *cvp;
        return cv;
    }

    bool parseBool(std::string value)
    {
        return value == "true";
    }

    int parseInt(std::string value)
    {
        return atoi(value.c_str());
    }

    double parseDouble(std::string value)
    {
        return atof(value.c_str());
    }

    Data* parseData(std::string key, std::string value)
    {
        if (isString(value))
        {
            return new StringData(key, parseString(value));
        }
        if (isChar(value))
        {
            return new CharData(key, parseChar(value));
        }
        if (isBool(value))
        {
            return new BoolData(key, parseBool(value));
        }
        if (isDouble(value))
        {
            return new DoubleData(key, parseDouble(value));
        }
        if (isInt(value))
        {
            return new IntData(key, parseInt(value));
        }
        if (isArray(value))
        {
            auto data = std::vector<Data*>();
            value = value.substr(1, value.length() - 2);
            auto rawArr = split(value, SY_COMMA);
            for (auto ri : rawArr)
            {
                if (isString(ri))
                {
                    data.push_back(new StringData(key, parseString(ri)));
                }
                if (isChar(ri))
                {
                    data.push_back(new CharData(key, parseChar(ri)));
                }
                if (isBool(ri))
                {
                    data.push_back(new BoolData(key, parseBool(ri)));
                }
                if (isDouble(ri))
                {
                    data.push_back(new DoubleData(key, parseDouble(ri)));
                }
                if (isInt(ri))
                {
                    data.push_back(new IntData(key, parseInt(ri)));
                }
            }
            return new ArrayData(key, data);
        }
        return nullptr;
    }

    std::vector<Section*> parser(std::vector<std::string> tokens)
    {
        auto sections = std::vector<Section*>();

        for (size_t i = 0; i < tokens.size(); i++)
        {
            // verifica a abertura de uma nova section
            if (tokens[i] == TK_SECTION)
            {
                auto section = new Section(std::to_string(sections.size()));
                i++;
                // verifica se a tag irá possuir um nome
                if (tokens[i] == TK_NAME)
                {
                    i++;
                    // verifica se há algum token ao invés de um nome
                    // um nome não pode ficar vazio
                    if (isToken(tokens[i]))
                    {
                        return std::vector<Section*>();
                    }
                    // verifica se é string
                    else if (tokens[i][0] != SY_DQUOTE)
                    {
                        return std::vector<Section*>();
                    }
                    else
                    {
                        section->setName(parseString(tokens[i]));
                    }
                    i++;
                    // verifica se a tag name é fechada
                    if (isCloseToken(tokens[i]))
                    {
                        i++;
                    }
                    else
                    {
                        return std::vector<Section*>();
                    }
                }
                // busca os dados de uma section
                while (tokens[i] == TK_DATA)
                {
                    std::string key;
                    std::string value;
                    i++;
                    // verifica se exise uma key
                    if (isToken(tokens[i]))
                    {
                        return std::vector<Section*>();
                    }
                    key = tokens[i];
                    i++;
                    // varifica se existe o simbolo de atribuição
                    if (tokens[i] != std::string(1, SY_COLON))
                    {
                        return std::vector<Section*>();
                    }
                    i++;
                    // le o valor
                    if (isToken(tokens[i]))
                    {
                        return std::vector<Section*>();
                    }
                    value = tokens[i];
                    i += 2;
                    section->add(parseData(key, value));
                }
                sections.push_back(section);
            }
            else
            {
                // caso não seja uma tag de section, gera um erro
                return std::vector<Section*>();
            }
        }

        return sections;
    }

    File* compile(std::string data)
    {
        if (data == "") {
            return new File();
        }

        auto tokens = lexer(data);
        auto sections = parser(tokens);
        return new File(sections);
    }

    File* read(std::string path)
    {
        std::string data = "";
        std::string line = "";

        std::ifstream f;
        f.open(normalizePath(path));

        if (f.is_open()) {
            while (!f.eof())
            {
                getline(f, line);
                data += line + "\n";
            }
        }
        f.close();

        return compile(data);
    }

    void write(File* file, std::string path)
    {
        std::ofstream wf;
        wf.open(normalizePath(path));

        wf << FILE_HEADER << SY_NEWLINE << SY_NEWLINE;
        for (auto section : file->findAll())
        {
            wf << TK_SECTION << SY_NEWLINE;
            wf << FILE_TAB << TK_NAME << SY_DQUOTE << section->getName() << SY_DQUOTE << TK_CLOSE << SY_NEWLINE;
            for (auto data : section->findAll())
            {
                wf << FILE_TAB << TK_DATA << data->getKey() << SY_COLON;
                if (dynamic_cast<StringData*>(data) != nullptr)
                {
                    StringData* strData = (StringData*)data;
                    wf << SY_DQUOTE << strData->getValue() << SY_DQUOTE;
                }
                else if (dynamic_cast<CharData*>(data) != nullptr)
                {
                    CharData* charData = (CharData*)data;
                    wf << SY_QUOTE << charData->getValue() << SY_QUOTE;
                }
                else if (dynamic_cast<BoolData*>(data) != nullptr)
                {
                    BoolData* boolData = (BoolData*)data;
                    if (boolData->getValue()) wf << "true";
                    else wf << "false";
                }
                else if (dynamic_cast<DoubleData*>(data) != nullptr)
                {
                    DoubleData* doubleData = (DoubleData*)data;
                    wf << doubleData->getValue();
                }
                else if (dynamic_cast<IntData*>(data) != nullptr)
                {
                    IntData* intData = (IntData*)data;
                    wf << intData->getValue();
                }
                else if (dynamic_cast<ArrayData*>(data) != nullptr)
                {
                    ArrayData* arrayData = (ArrayData*)data;
                    std::vector<Data*> arrayValue = (std::vector<Data*>)arrayData->getValue();
                    wf << SY_OPEN_BRACKETS;
                    for (size_t i = 0; i < arrayValue.size(); i++)
                    {
                        Data* curData = arrayValue[i];
                        if (dynamic_cast<StringData*>(curData) != nullptr)
                        {
                            StringData* strData = (StringData*)curData;
                            wf << SY_DQUOTE << strData->getValue() << SY_DQUOTE;
                        }
                        else if (dynamic_cast<CharData*>(curData) != nullptr)
                        {
                            CharData* charData = (CharData*)curData;
                            wf << SY_QUOTE << charData->getValue() << SY_QUOTE;
                        }
                        else if (dynamic_cast<BoolData*>(curData) != nullptr)
                        {
                            BoolData* boolData = (BoolData*)curData;
                            if (boolData->getValue()) wf << "true";
                            else wf << "false";
                        }
                        else if (dynamic_cast<DoubleData*>(curData) != nullptr)
                        {
                            DoubleData* doubleData = (DoubleData*)curData;
                            wf << doubleData->getValue();
                        }
                        else if (dynamic_cast<IntData*>(curData) != nullptr)
                        {
                            IntData* intData = (IntData*)curData;
                            wf << intData->getValue();
                        }
                        if (i < arrayValue.size() - 1) wf << SY_COMMA;
                    }
                    wf << SY_CLOSE_BRACKETS;
                }
                wf << TK_CLOSE << SY_NEWLINE;
            }
            wf << TK_CLOSE << SY_NEWLINE << SY_NEWLINE;
        }
        wf.close();
    }
}

#endif // BPS_H
