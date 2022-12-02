#pragma once
#include <vulkan/vulkan_core.h>

#include <vector>

struct TSBufferResource;

class TComputePass
{
public:
    TComputePass(VkDevice _device) :device(_device) { x = y = z = groupNum = 0; }
    ~TComputePass() {}

    void SetComputeConfig(uint32_t _x, uint32_t _y, uint32_t _z, uint32_t _groupNum);
    void Dispatch(VkCommandBuffer _commBuffer);
    void Dispose();

    static void CreateComputeDescriptorPool(VkDevice _device);
    static void DestroyComputePassHandles(VkDevice _device);
    static TComputePass* CreateComputePass(VkDevice _device);


    void CreateDescriptorSetLayout(size_t _ssboCount);
    void CreateComputePipeline();


    void AddStorageBuffers(VkDeviceSize _size, void* _data);
    void CreateDescriptorSet();
    
private:
    VkDevice device;

    VkDescriptorSetLayout   descriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorSet         descriptorSet = VK_NULL_HANDLE;
    VkPipelineLayout        pipelineLayout = VK_NULL_HANDLE;
    VkPipeline              computePipeline = VK_NULL_HANDLE;
    uint32_t                x, y, z, groupNum;

    std::vector<TSBufferResource>   storageBuffers;
};

