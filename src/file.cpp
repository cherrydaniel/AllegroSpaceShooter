#include "file.h"

unsigned char fileReadU8(std::istream& fs)
{
    unsigned char v;
    fs.read((char*)&v, 1);
    return v;
}

unsigned short fileReadU16(std::istream& fs)
{
    unsigned char bytes[2];
    fs.read((char*)bytes, 2);
    return (bytes[1]<<8)|bytes[0];
}

unsigned long fileReadU32(std::istream& fs)
{
    unsigned char bytes[4];
    fs.read((char*)bytes, 4);
    return (bytes[3]<<24)|(bytes[2]<<16)|(bytes[1]<<8)|bytes[0];
}

std::string fileReadString(std::istream& fs)
{
    unsigned long len = fileReadU32(fs);
    char* buf = new char[len];
    fs.read(buf, len);
    std::string v{buf, len};
    delete[] buf;
    return v;
}

std::string fileReadAllAsString(std::istream& fs)
{
    std::stringstream ss;
    ss << fs.rdbuf();
    return ss.str();
}

std::string fileReadAllAsString(const char* path, bool& err)
{
    std::ifstream fs{path};
    if (!fs.is_open())
    {
        err = true;
        return "";
    }
    err = false;
    return fileReadAllAsString(fs);
}

void fileWriteU8(std::ostream& fs, unsigned char v)
{
    fs.write((char*)&v, 1);
}

void fileWriteU16(std::ostream& fs, unsigned short v)
{
    unsigned char bytes[2];
    for (unsigned int i = 0; i<2; i++)
        bytes[i] = (v>>(8*i))&0xFF;
    fs.write((char*)bytes, 2);
}

void fileWriteU32(std::ostream& fs, unsigned long v)
{
    unsigned char bytes[4];
    for (unsigned int i = 0; i<4; i++)
        bytes[i] = (v>>(8*i))&0xFF;
    fs.write((char*)bytes, 4);
}

void fileWriteString(std::ostream& fs, std::string v)
{
    unsigned long len = v.length();
    fileWriteU32(fs, len);
    fs.write(v.c_str(), len);
}
