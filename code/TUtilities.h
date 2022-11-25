#pragma once

#include <vector>
#include <string>
#include <functional>

class TUtilities
{
public:
    static std::vector<char> readFile(const std::string& filename);
    template<typename T>
    static size_t readFile(const std::string& filename, T& _buffer);

    static bool writeFile(const std::string& filename, void* _buffer, size_t _size);
private:
    static size_t _readFile(const std::string& filename, std::function<void*(size_t)> _resizeFunc);
};

template<typename T>
inline size_t TUtilities::readFile(const std::string& filename, T& _buffer)
{
    return _readFile(filename, [&_buffer](size_t _size) -> void* { _buffer.resize(_size); return _buffer.data(); });
}
