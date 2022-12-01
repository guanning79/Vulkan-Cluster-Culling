#include "TBufferManager.h"

void TBufferManager::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
    //VkBufferCreateInfo bufferInfo{};
    //bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    //bufferInfo.size = size;
    //bufferInfo.usage = usage;
    //bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    //if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
    //    throw std::runtime_error("failed to create buffer!");
    //}

    //VkMemoryRequirements memRequirements;
    //vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

    //VkMemoryAllocateInfo allocInfo{};
    //allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    //allocInfo.allocationSize = memRequirements.size;
    //allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    //if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
    //    throw std::runtime_error("failed to allocate buffer memory!");
    //}

    //vkBindBufferMemory(device, buffer, bufferMemory, 0);
}
