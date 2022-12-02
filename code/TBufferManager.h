#pragma once

#include <vulkan/vulkan_core.h>

struct TSBufferResource
{
    VkBuffer            buffer;
    VkDeviceMemory      memory;
    size_t              size;
};

class TBufferManager
{
public:
    static void createBuffer(VkDevice _device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory); 
    static void InitContext(VkPhysicalDevice _pdevice);
};

