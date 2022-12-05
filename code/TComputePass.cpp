
#include <stdexcept>

#include <array>

#include "TBufferManager.h"
#include "TShaderModuleCreateHelper.h"

#include "TComputePass.h"

#define DESCRIPTOR_POOL_UNIFORM_BUFFER_COUNT    10
#define DESCRIPTOR_POOL_STORAGE_BUFFER_COUNT    10
#define DESCRIPTOR_POOL_MAX_DESCRIPTORSET_COUNT    10

VkDescriptorPool SCompDescriptorPool = VK_NULL_HANDLE;
std::vector<TComputePass*>  SComputePasses;

void TComputePass::CreateComputeDescriptorPool(VkDevice _device)
{
    std::array<VkDescriptorPoolSize, 2> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = DESCRIPTOR_POOL_UNIFORM_BUFFER_COUNT;
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    poolSizes[1].descriptorCount = DESCRIPTOR_POOL_STORAGE_BUFFER_COUNT;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = DESCRIPTOR_POOL_MAX_DESCRIPTORSET_COUNT;

    if (vkCreateDescriptorPool(_device, &poolInfo, nullptr, &SCompDescriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

void TComputePass::DestroyComputePassHandles(VkDevice _device)
{
    if(SCompDescriptorPool != VK_NULL_HANDLE)
        vkDestroyDescriptorPool(_device, SCompDescriptorPool, nullptr);

    for (size_t i=0; i<SComputePasses.size(); i++)
    {
        SComputePasses[i]->Dispose();
    }
    SComputePasses.clear();
}

TComputePass* TComputePass::CreateComputePass(VkDevice _device)
{
    SComputePasses.push_back(new TComputePass(_device));
    return SComputePasses[SComputePasses.size() - 1];
}

void TComputePass::CreateDescriptorSetLayout(size_t _ssboCount)
{
    std::vector<VkDescriptorSetLayoutBinding> bindings;
    bindings.resize(_ssboCount);

    for (size_t i = 0; i < _ssboCount; i++)
    {
        VkDescriptorSetLayoutBinding layoutBinding{};
        layoutBinding.binding = (uint32_t)i;
        layoutBinding.descriptorCount = 1;
        layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        layoutBinding.pImmutableSamplers = nullptr;
        layoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
        bindings[i] = layoutBinding;
    }

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

void TComputePass::CreateComputePipeline()
{
    VkPipelineKey pipelineLayoutKey = GetPipelineCreateHelper().GetVkPipelineLayout(device, descriptorSetLayout, pipelineLayout);
    VkPipelineKey pipelineKey;
    TPipelineCreateHelper::ComputePipelineStats pipelineStats = {};
    pipelineStats.csEntry = "main";
    pipelineStats.csName = "shaders/cluster_assembly_comp.spv";
    pipelineStats.pipelineLayoutKey = pipelineLayoutKey;
    computePipeline = GetPipelineCreateHelper().GetVkComputePipeline(device, pipelineStats, pipelineKey);
}

void TComputePass::Dispose()
{
    vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);

    for (size_t i=0; i< storageBuffers.size(); i++)
    {
        vkDestroyBuffer(device, storageBuffers[i].buffer, nullptr);
        vkFreeMemory(device, storageBuffers[i].memory, nullptr);
    }
}

void TComputePass::AddStorageBuffers(VkDeviceSize _size, void* _data, VkBufferUsageFlags _addtionalFlags /* = 0 */)
{
    TSBufferResource bufferResource = {};
    bufferResource.size = (size_t)_size;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    TBufferManager::createBuffer(device, _size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* dataStaging;
    vkMapMemory(device, stagingBufferMemory, 0, _size, 0, &dataStaging);
    if (_data != nullptr)
        memcpy(dataStaging, _data, bufferResource.size);
    else
        memset(dataStaging, 1, bufferResource.size);
    vkUnmapMemory(device, stagingBufferMemory);

    VkBufferUsageFlags flags = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    flags |= _addtionalFlags;
    TBufferManager::createBuffer(device, _size, flags, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, bufferResource.buffer, bufferResource.memory);

    extern void CopyVkBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    CopyVkBuffer(stagingBuffer, bufferResource.buffer, bufferResource.size);

    storageBuffers.push_back(bufferResource);

    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);
}

void TComputePass::CreateDescriptorSet()
{
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = SCompDescriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &descriptorSetLayout;

    vkAllocateDescriptorSets(device, &allocInfo, &descriptorSet);

    std::vector<VkWriteDescriptorSet> descriptorWrites(storageBuffers.size());
    std::vector<VkDescriptorBufferInfo> bufferInfos(storageBuffers.size());
    for (size_t i=0; i<storageBuffers.size(); i++)
    {
        bufferInfos[i].buffer = storageBuffers[i].buffer;
        bufferInfos[i].offset = 0;
        bufferInfos[i].range = storageBuffers[i].size;

        VkWriteDescriptorSet write = {};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet = descriptorSet;
        write.dstBinding = (uint32_t)i;
        write.dstArrayElement = 0;
        write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        write.descriptorCount = 1;
        write.pBufferInfo = &bufferInfos[i];
        descriptorWrites[i] = write;
    }

    vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
}

void TComputePass::Dispatch(VkCommandBuffer _commBuffer)
{
    //Bind compute pipeline
    //Bind descriptor set at the VK_PIPELINE_BIND_POINT_COMPUTE
    //Dispatch
    vkCmdBindPipeline(_commBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, computePipeline);
    vkCmdBindDescriptorSets(_commBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
    vkCmdDispatch(_commBuffer, x, y, z);
}

void TComputePass::SetComputeConfig(uint32_t _x, uint32_t _y, uint32_t _z, uint32_t _groupNum)
{
    x = _x; y = _y; z = _z; groupNum = _groupNum;
}