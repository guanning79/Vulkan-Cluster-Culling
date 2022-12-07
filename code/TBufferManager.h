#pragma once

#include <string>
#include <vulkan/vulkan_core.h>

struct TSBufferResource
{
    VkBuffer            buffer;
    VkDeviceMemory      memory;
    size_t              size;
    std::string         name;
};

class TBufferManager
{
public:
    static void createBuffer(VkDevice _device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory); 
    static void InitContext(VkPhysicalDevice _pdevice);
};

