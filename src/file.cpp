#include "file.h"

FileWriter::FileWriter(const char* path){ fs.open(path); }

FileWriter::~FileWriter(){ fs.close(); }

bool FileWriter::isOpen(){ return fs.is_open(); }

void FileWriter::writeU8(unsigned char v)
{
    fs.write((char*)&v, 1);
}

void FileWriter::writeU16(unsigned short v)
{
    unsigned char bytes[2];
    for (unsigned int i = 0; i<2; i++)
        bytes[i] = (v>>(8*i))&0xFF;
    fs.write((char*)bytes, 2);
}

void FileWriter::writeU32(unsigned long v)
{
    unsigned char bytes[4];
    for (unsigned int i = 0; i<4; i++)
        bytes[i] = (v>>(8*i))&0xFF;
    fs.write((char*)bytes, 4);
}

void FileWriter::writeString(std::string v)
{
    unsigned long len = v.length();
    writeU32(len);
    fs.write(v.c_str(), len);
}

FileReader::FileReader(const char* path){ fs.open(path); }

FileReader::~FileReader(){ fs.close(); }

bool FileReader::isOpen(){ return fs.is_open(); }

unsigned char FileReader::readU8()
{
    unsigned char v;
    fs.read((char*)&v, 1);
    return v;
}

unsigned short FileReader::readU16()
{
    unsigned char bytes[2];
    fs.read((char*)bytes, 2);
    return (bytes[1]<<8)|bytes[0];
}

unsigned long FileReader::readU32()
{
    unsigned char bytes[4];
    fs.read((char*)bytes, 4);
    return (bytes[3]<<24)|(bytes[2]<<16)|(bytes[1]<<8)|bytes[0];
}

std::string FileReader::readString()
{
    size_t len = static_cast<size_t>(readU32());
    std::string v(len, '\0');
    fs.read(&v[0], len);
    return v;
}

std::string FileReader::readAllAsString()
{
    std::stringstream ss;
    ss << fs.rdbuf();
    return ss.str();
}
