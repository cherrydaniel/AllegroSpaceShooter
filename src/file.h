#ifndef _GAME_FILE_H_
#define _GAME_FILE_H_

#include <fstream>
#include <string>
#include <sstream>

unsigned char fileReadU8(std::istream& fs);

unsigned short fileReadU16(std::istream& fs);

unsigned long fileReadU32(std::istream& fs);

std::string fileReadString(std::istream& fs);

std::string fileReadAllAsString(std::istream& fs);

std::string fileReadAllAsString(const char* path, bool& err);

void fileWriteU8(std::ostream& fs, unsigned char v);

void fileWriteU16(std::ostream& fs, unsigned short v);

void fileWriteU32(std::ostream& fs, unsigned long v);

void fileWriteString(std::ostream& fs, std::string v);

#endif
