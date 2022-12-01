#pragma once

#include <vulkan/vulkan_core.h>

class TBufferManager
{
public:
    static void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory); 
};

