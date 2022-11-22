#include <stdexcept>

#include <array>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#include "TUtilities.h"

#include "TShaderModuleCreateHelper.h"

#pragma warning(disable: 26812)

TShaderModuleCreateHelper::TShaderModuleCreateHelper(VkDevice _device, const char* _path, const VkShaderStageFlagBits& _shaderStage, const char* _entry): device(_device), shaderModule(VK_NULL_HANDLE)
{
    auto shaderCode = TUtilities::readFile(_path);

    shaderModule = CreateShaderModule(device, shaderCode);

    shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageInfo.stage = _shaderStage;
    shaderStageInfo.module = shaderModule;
    shaderStageInfo.pName = _entry;
}

TShaderModuleCreateHelper::~TShaderModuleCreateHelper()
{
    Dispose();
}

void TShaderModuleCreateHelper::Dispose()
{
    if (shaderModule != VK_NULL_HANDLE) {
        vkDestroyShaderModule(device, shaderModule, nullptr);
        shaderModule = VK_NULL_HANDLE;
    }
}

VkShaderModule TShaderModuleCreateHelper::CreateShaderModule(VkDevice _device, const std::vector<char>& code) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule module;
    if (vkCreateShaderModule(_device, &createInfo, nullptr, &module) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }

    return module;
}

VkPipelineKey sKeyCounter = 0;
VkPipelineKey TPipelineCreateHelper::PipelineStats::getPipelineKey() const
{
    return sKeyCounter++;
}

#define PIPELINE_INFO_FUNC_DEFINE(info, ...)    void TPipelineCreateHelper::_get##info(VkPipeline##info& __VA_ARGS__)

PIPELINE_INFO_FUNC_DEFINE(RasterizationStateCreateInfo, rasterizer, VkPolygonMode _pmode, float _lineWidth)
{
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = _pmode;
    rasterizer.lineWidth = _lineWidth;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
}

PIPELINE_INFO_FUNC_DEFINE(MultisampleStateCreateInfo, multisampling, VkSampleCountFlagBits _bits)
{
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = _bits;
}

PIPELINE_INFO_FUNC_DEFINE(DepthStencilStateCreateInfo, depthStencil)
{
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;
}

struct VertexDefine {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(VertexDefine);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(VertexDefine, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(VertexDefine, color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(VertexDefine, texCoord);

        return attributeDescriptions;
    }

    bool operator==(const VertexDefine& other) const {
        return pos == other.pos && color == other.color && texCoord == other.texCoord;
    }
};

void TPipelineCreateHelper::Dispose(VkDevice _device)
{
    if (pipelineCache != VK_NULL_HANDLE)
    {
        vkDestroyPipelineCache(_device, pipelineCache, nullptr);
    }
}

VkPipeline TPipelineCreateHelper::GetVkPipeline(VkDevice _device, const PipelineStats& _pipelineStats, VkPipelineKey& _keyOut)
{
    _keyOut = _pipelineStats.getPipelineKey();
    if (pipelineMap.find(_keyOut) != pipelineMap.end())
        return pipelineMap[_keyOut];

    TShaderModuleCreateHelper vert(_device, _pipelineStats.vsName.c_str(), VK_SHADER_STAGE_VERTEX_BIT, _pipelineStats.vsEntry.c_str());
    TShaderModuleCreateHelper frag(_device, _pipelineStats.fsName.c_str(), VK_SHADER_STAGE_FRAGMENT_BIT, _pipelineStats.fsEntry.c_str());

    VkPipelineShaderStageCreateInfo shaderStages[] = { vert.GetShaderStageInfo(), frag.GetShaderStageInfo() };

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    auto bindingDescription = VertexDefine::getBindingDescription();
    auto attributeDescriptions = VertexDefine::getAttributeDescriptions();

    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    
    VkViewport viewport = {};
    viewport.x = _pipelineStats.vpx;
    viewport.y = _pipelineStats.vpy;
    viewport.width = (float)_pipelineStats.swapChainExtent.width;
    viewport.height = (float)_pipelineStats.swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = {};
    scissor.offset = { 0, 0 };
    scissor.extent = _pipelineStats.swapChainExtent;

    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    _getRasterizationStateCreateInfo(rasterizer, _pipelineStats.polygonMode, 1.0f);

    VkPipelineMultisampleStateCreateInfo multisampling = {};
    _getMultisampleStateCreateInfo(multisampling, _pipelineStats.sampleCountFlags);

    VkPipelineDepthStencilStateCreateInfo depthStencil = {};
    _getDepthStencilStateCreateInfo(depthStencil);

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    VkPipelineLayout pipelineLayout = GetVkPipelineLayout(_pipelineStats.descSetLayoutKey);

    VkGraphicsPipelineCreateInfo pipelineInfos = {};

    pipelineInfos.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfos.flags |= (!_pipelineStats.isDerivatePipeline) ? VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT : VK_PIPELINE_CREATE_DERIVATIVE_BIT;
    pipelineInfos.stageCount = 2;
    pipelineInfos.pStages = shaderStages;
    pipelineInfos.pVertexInputState = &vertexInputInfo;
    pipelineInfos.pInputAssemblyState = &inputAssembly;
    pipelineInfos.pViewportState = &viewportState;
    pipelineInfos.pRasterizationState = &rasterizer;
    pipelineInfos.pMultisampleState = &multisampling;
    pipelineInfos.pDepthStencilState = &depthStencil;
    pipelineInfos.pColorBlendState = &colorBlending;
    pipelineInfos.layout = pipelineLayout;
    pipelineInfos.renderPass = _pipelineStats.renderPass;
    pipelineInfos.subpass = 0;
    pipelineInfos.basePipelineHandle = (!_pipelineStats.isDerivatePipeline) ? VK_NULL_HANDLE : _pipelineStats.basePipeline;

    VkPipeline pipeline;
    VkResult res = vkCreateGraphicsPipelines(_device, pipelineCache, 1, &pipelineInfos, nullptr, &pipeline);
    if (res != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics pipeline!");
    }
    pipelineMap[_keyOut] = pipeline;
    return pipeline;
}

VkPipelineKey TPipelineCreateHelper::GetVkPipelineLayout(VkDevice _device, VkDescriptorSetLayout _descSetLayout, VkPipelineLayout& _outPipelineLayout)
{
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &_descSetLayout;

    VkPipelineKey key = INVALID_PIPELINE_KEY;
    if (vkCreatePipelineLayout(_device, &pipelineLayoutInfo, nullptr, &_outPipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }
    else
    {
        key = (VkPipelineKey)pipelineLayoutMap.size();
        pipelineLayoutMap[key] = _outPipelineLayout;
    }
    return key;
}

void TPipelineCreateHelper::Init(VkDevice _device)
{
    size_t cacheFileSize;
    void* data = nullptr;
    int res = _loadPipelineCache(cacheFileSize, data);

    VkPipelineCacheCreateInfo pipelineCacheInfo = {};
    pipelineCacheInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    if (res = -1)
    {
        //load pipeline file failed
        pipelineCacheInfo.initialDataSize = 0;
        pipelineCacheInfo.pInitialData = nullptr;
    }
    else
    {
        pipelineCacheInfo.initialDataSize = cacheFileSize;
        pipelineCacheInfo.pInitialData = data;
    }
    vkCreatePipelineCache(_device, &pipelineCacheInfo, nullptr, &pipelineCache);
}

void TPipelineCreateHelper::_cleanUp()
{
    pipelineMap.clear(); 
    pipelineLayoutMap.clear();
}

void TPipelineCreateHelper::_exportPipelineCache()
{

}

int TPipelineCreateHelper::_loadPipelineCache(size_t& _size, void*& _data)
{
    return 0;
}
