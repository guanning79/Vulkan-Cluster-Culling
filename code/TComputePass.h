#pragma once
#include <vulkan/vulkan_core.h>

#include <vector>

class TComputePass
{
public:
    TComputePass(VkDevice _device) :device(_device) {}
    ~TComputePass() {}

    void Dispatch(VkCommandBuffer _commBuffer) {}

    void CreateDescriptorSetLayout(size_t _ssboCount);

private:
    VkDevice device;

    VkDescriptorSetLayout   descriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorSet         descriptorSet = VK_NULL_HANDLE;
    VkPipelineLayout        pipelineLayout = VK_NULL_HANDLE;
    VkPipeline              computePipeline = VK_NULL_HANDLE;

    std::vector<VkBuffer>   storageBuffers;
};

