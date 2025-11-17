#ifndef _GAME_FILE_H_
#define _GAME_FILE_H_

#include <fstream>
#include <string>
#include <sstream>

class FileWriter
{
private:
    std::ofstream fs;
public:
    FileWriter(const char* path);
    ~FileWriter();
    bool isOpen();
    void writeU8(unsigned char v);
    void writeU16(unsigned short v);
    void writeU32(unsigned long v);
    void writeString(std::string v);
};

class FileReader
{
private:
    std::ifstream fs;
public:
    FileReader(const char* path);
    ~FileReader();
    bool isOpen();
    unsigned char readU8();
    unsigned short readU16();
    unsigned long readU32();
    std::string readString();
    std::string readAllAsString();
};

#endif
