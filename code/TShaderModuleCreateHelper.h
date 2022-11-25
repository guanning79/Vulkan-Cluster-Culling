#pragma once

#include <vulkan/vulkan_core.h>

#include <vector>
#include <map>
#include <string>

class TShaderModuleCreateHelper
{
public:
    TShaderModuleCreateHelper(VkDevice _device, const char* _path, const VkShaderStageFlagBits& _shaderStage, const char* _entry);
    ~TShaderModuleCreateHelper();

    inline VkPipelineShaderStageCreateInfo& GetShaderStageInfo() { return shaderStageInfo; }
    void Dispose();

    static VkShaderModule CreateShaderModule(VkDevice _device, const std::vector<char>& code);

private:
    VkShaderModule  shaderModule;
    VkDevice        device;
    VkPipelineShaderStageCreateInfo shaderStageInfo;
};

#define PIPELINE_INFO_FUNC_DECAL(info, ...)    void _get##info(VkPipeline##info& __VA_ARGS__)
typedef int64_t VkPipelineKey;
#define INVALID_PIPELINE_KEY    -1

class TPipelineCreateHelper
{
public: 
    TPipelineCreateHelper():pipelineCache(VK_NULL_HANDLE) { }
    ~TPipelineCreateHelper() { _cleanUp(); }

    void    Init(VkDevice _device);
    void    Dispose(VkDevice _device);

    struct PipelineStats 
    {
        std::string             vsName, vsEntry;
        std::string             fsName, fsEntry;
        VkExtent2D              swapChainExtent;
        float                   vpx, vpy;
        VkSampleCountFlagBits   sampleCountFlags;
        VkPipelineKey           descSetLayoutKey;
        bool                    isDerivatePipeline;
        VkPipeline              basePipeline;
        VkPolygonMode           polygonMode;
        float                   wireframeLineWidth;
        VkRenderPass            renderPass;

        VkPipelineKey           getPipelineKey() const;
    };

    //VkExtent2D SwapChainExtentRef;

    VkPipeline          GetVkPipeline(VkDevice _device, const PipelineStats& _pipelineStats, VkPipelineKey& _keyOut);
    VkPipelineLayout    GetVkPipelineLayout(VkPipelineKey _key) { return (pipelineLayoutMap.find(_key) != pipelineLayoutMap.end()) ? pipelineLayoutMap[_key] : VK_NULL_HANDLE; }
    VkPipelineKey       GetVkPipelineLayout(VkDevice _device, VkDescriptorSetLayout _descSetLayout, VkPipelineLayout& _outPipelineLayout);

private:
    PIPELINE_INFO_FUNC_DECAL(RasterizationStateCreateInfo, rasterizer, VkPolygonMode _pmode, float _lineWidth);
    PIPELINE_INFO_FUNC_DECAL(MultisampleStateCreateInfo, multisampling, VkSampleCountFlagBits _bits);
    PIPELINE_INFO_FUNC_DECAL(DepthStencilStateCreateInfo, depthStencil);

    void    _cleanUp();
    void    _exportPipelineCache();

    std::map<VkPipelineKey, VkPipeline>         pipelineMap;
    std::map<VkPipelineKey, VkPipelineLayout>   pipelineLayoutMap;
    VkPipelineCache                             pipelineCache;
};
