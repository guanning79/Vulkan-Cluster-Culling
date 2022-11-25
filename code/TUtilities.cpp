
#include <iostream>
#include <fstream>

#include "TUtilities.h"

std::vector<char> TUtilities::readFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

bool TUtilities::writeFile(const std::string& filename, void* _buffer, size_t _size)
{
    std::ofstream file(filename, std::ios::ate | std::ios::binary | std::ios::trunc);
    if (!file.is_open())
        return false;

    file.write((char *)_buffer, _size);
    file.close();
    return true;
}

size_t TUtilities::_readFile(const std::string& filename, std::function<void*(size_t)> _resizeFunc)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        return 0;
    }

    size_t fileSize = (size_t)file.tellg();
    void* buffer = _resizeFunc(fileSize);

    file.seekg(0);
    file.read((char*)buffer, fileSize);
    file.close();

    return fileSize;
}
