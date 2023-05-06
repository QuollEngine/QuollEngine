#include "liquid/core/Base.h"
#include "liquid/renderer/RenderGraph.h"
#include "liquid/renderer/TextureUtils.h"

#include "HDRIImporter.h"

#include <stb/stb_image.h>

namespace liquid::editor {

static constexpr uint32_t CubemapSides = 6;

static constexpr rhi::Format TargetFormat = rhi::Format::Rgba16Float;
static constexpr uint32_t FormatSize = 4 * 2;

HDRIImporter::HDRIImporter(AssetCache &assetCache, RenderStorage &renderStorage)
    : mAssetCache(assetCache), mRenderStorage(renderStorage) {

  auto *device = mRenderStorage.getDevice();

  const auto shadersPath =
      std::filesystem::current_path() / "assets" / "shaders";

  {
    rhi::DescriptorLayoutBindingDescription binding0{};
    binding0.type = rhi::DescriptorLayoutBindingType::Static;
    binding0.binding = 0;
    binding0.name = "uInputTexture";
    binding0.shaderStage = rhi::ShaderStage::Compute;
    binding0.descriptorCount = 1;
    binding0.descriptorType = rhi::DescriptorType::CombinedImageSampler;

    rhi::DescriptorLayoutBindingDescription binding1{};
    binding1.type = rhi::DescriptorLayoutBindingType::Static;
    binding1.binding = 1;
    binding1.name = "uOutputTexture";
    binding1.shaderStage = rhi::ShaderStage::Compute;
    binding1.descriptorCount = 1;
    binding1.descriptorType = rhi::DescriptorType::StorageImage;

    auto layout = device->createDescriptorLayout({{binding0, binding1}});
    mDescriptorGenerateCubemap = device->createDescriptor(layout);
  }

  {
    auto shader = device->createShader(
        {shadersPath / "equirectangular-to-cubemap.comp.spv"});

    mPipelineGenerateCubemap =
        mRenderStorage.addPipeline(rhi::ComputePipelineDescription{shader});

    mRenderStorage.getDevice()->createPipeline(
        mRenderStorage.getComputePipelineDescription(mPipelineGenerateCubemap),
        mPipelineGenerateCubemap);
  }

  {
    auto shader = device->createShader(
        {shadersPath / "generate-irradiance-map.comp.spv"});

    mPipelineGenerateIrradianceMap =
        mRenderStorage.addPipeline(rhi::ComputePipelineDescription{shader});

    mRenderStorage.getDevice()->createPipeline(
        mRenderStorage.getComputePipelineDescription(
            mPipelineGenerateIrradianceMap),
        mPipelineGenerateIrradianceMap);
  }

  {
    auto shader =
        device->createShader({shadersPath / "generate-specular-map.comp.spv"});

    mPipelineGenerateSpecularMap =
        mRenderStorage.addPipeline(rhi::ComputePipelineDescription{shader});

    mRenderStorage.getDevice()->createPipeline(
        mRenderStorage.getComputePipelineDescription(
            mPipelineGenerateSpecularMap),
        mPipelineGenerateSpecularMap);
  }
}

Result<Path> HDRIImporter::loadFromPath(const Path &originalAssetPath,
                                        const Path &engineAssetPath) {
  auto *device = mRenderStorage.getDevice();

  int32_t width = 0;
  int32_t height = 0;
  int32_t channels = 0;

  auto *data = stbi_loadf(originalAssetPath.string().c_str(), &width, &height,
                          &channels, STBI_rgb_alpha);

  if (!data) {
    return Result<Path>::Error(stbi_failure_reason());
  }

  std::filesystem::create_directory(engineAssetPath);

  auto relPath =
      std::filesystem::relative(engineAssetPath, mAssetCache.getAssetsPath());

  auto unfilteredCubemap = convertEquirectangularToCubemap(
      data, static_cast<uint32_t>(width), static_cast<uint32_t>(height));
  stbi_image_free(data);

  auto irradianceMapName = originalAssetPath.stem().string() + "-irradiance";
  auto irradianceCubemap =
      generateIrradianceMap(unfilteredCubemap, relPath / irradianceMapName);

  if (irradianceCubemap.hasError()) {
    device->destroyTexture(unfilteredCubemap.texture);
    std::filesystem::remove_all(engineAssetPath);

    return Result<Path>::Error(irradianceCubemap.getError());
  }

  auto specularMapName = originalAssetPath.stem().string() + "-specular";
  auto specularCubemap =
      generateSpecularMap(unfilteredCubemap, relPath / specularMapName);

  if (specularCubemap.hasError()) {
    device->destroyTexture(unfilteredCubemap.texture);
    mAssetCache.getRegistry().getTextures().deleteAsset(
        irradianceCubemap.getData());
    std::filesystem::remove_all(engineAssetPath);

    return Result<Path>::Error(specularCubemap.getError());
  }

  device->destroyTexture(unfilteredCubemap.texture);

  AssetData<EnvironmentAsset> environment{};
  environment.path =
      engineAssetPath / originalAssetPath.filename().replace_extension("lqenv");
  environment.data.specularMap = specularCubemap.getData();
  environment.data.irradianceMap = irradianceCubemap.getData();

  auto createdFileRes = mAssetCache.createEnvironmentFromAsset(environment);

  if (createdFileRes.hasError()) {
    return createdFileRes;
  }

  auto loadRes = mAssetCache.loadAsset(createdFileRes.getData());
  if (loadRes.hasError()) {
    return Result<Path>::Error(loadRes.getError());
  }

  return createdFileRes;
}

rhi::TextureHandle
HDRIImporter::loadFromPathToDevice(const Path &originalAssetPath,
                                   RenderStorage &renderStorage) {
  auto *device = mRenderStorage.getDevice();

  int32_t width = 0;
  int32_t height = 0;
  int32_t channels = 0;

  auto *data = stbi_loadf(originalAssetPath.string().c_str(), &width, &height,
                          &channels, STBI_rgb_alpha);

  if (!data) {
    return rhi::TextureHandle::Invalid;
  }

  rhi::TextureDescription hdriTextureDesc{};
  hdriTextureDesc.usage = rhi::TextureUsage::Color |
                          rhi::TextureUsage::TransferDestination |
                          rhi::TextureUsage::Sampled;
  hdriTextureDesc.format = rhi::Format::Rgba32Float;
  hdriTextureDesc.width = width;
  hdriTextureDesc.height = height;
  hdriTextureDesc.layers = 1;

  auto hdriTexture = renderStorage.createTexture(hdriTextureDesc);
  TextureUtils::copyDataToTexture(
      device, data, hdriTexture, rhi::ImageLayout::ShaderReadOnlyOptimal, 1,
      {{0, static_cast<size_t>(width) * height * 4 * sizeof(float),
        static_cast<uint32_t>(width), static_cast<uint32_t>(height)}});

  return hdriTexture;
}

HDRIImporter::CubemapData
HDRIImporter::convertEquirectangularToCubemap(float *data, uint32_t width,
                                              uint32_t height) {
  auto *device = mRenderStorage.getDevice();

  rhi::TextureDescription hdriTextureDesc{};
  hdriTextureDesc.usage = rhi::TextureUsage::Color |
                          rhi::TextureUsage::TransferDestination |
                          rhi::TextureUsage::Sampled;
  hdriTextureDesc.format = rhi::Format::Rgba32Float;
  hdriTextureDesc.width = width;
  hdriTextureDesc.height = height;
  hdriTextureDesc.layers = 1;

  auto hdriTexture = mRenderStorage.createTexture(hdriTextureDesc, false);

  TextureUtils::copyDataToTexture(
      device, data, hdriTexture, rhi::ImageLayout::ShaderReadOnlyOptimal, 1,
      {{0, static_cast<size_t>(width) * height * 4 * sizeof(float),
        static_cast<uint32_t>(width), static_cast<uint32_t>(height)}});

  const uint32_t CubemapResolution = height / 2;
  const uint32_t CubemapFaceSize =
      CubemapResolution * CubemapResolution * FormatSize;
  const size_t CubemapFullSize = CubemapFaceSize * CubemapSides;
  const uint32_t GroupCount = CubemapResolution / 32;

  auto numLevels =
      static_cast<uint32_t>(std::floor(std::log2(CubemapResolution))) + 1;

  std::vector<TextureAssetLevel> levels(numLevels);
  uint32_t bufferOffset = 0;
  uint32_t mipWidth = CubemapResolution;
  uint32_t mipHeight = CubemapResolution;
  for (uint32_t i = 0; i < numLevels; ++i) {
    levels.at(i).offset = bufferOffset;
    levels.at(i).size =
        static_cast<size_t>(mipWidth) * mipHeight * CubemapSides * FormatSize;
    levels.at(i).width = mipWidth;
    levels.at(i).height = mipHeight;

    bufferOffset += static_cast<uint32_t>(levels.at(i).size);

    if (mipWidth > 1) {
      mipWidth /= 2;
    }

    if (mipHeight > 1) {
      mipHeight /= 2;
    }
  }

  size_t totalSize = TextureUtils::getBufferSizeFromLevels(levels);

  // Unfiltered cubemap, prefiltered cubemap,
  // irradiance map and specular map all have the same
  // texture spec
  rhi::TextureDescription cubemapDesc;
  cubemapDesc.type = rhi::TextureType::Cubemap;
  cubemapDesc.format = TargetFormat;
  cubemapDesc.height = CubemapResolution;
  cubemapDesc.width = CubemapResolution;
  cubemapDesc.layers = CubemapSides;
  cubemapDesc.levels = numLevels;
  cubemapDesc.usage = rhi::TextureUsage::Color | rhi::TextureUsage::Storage |
                      rhi::TextureUsage::Sampled |
                      rhi::TextureUsage::TransferSource |
                      rhi::TextureUsage::TransferDestination;

  auto unfilteredCubemap = mRenderStorage.createTexture(cubemapDesc);

  // Convert equirectangular texture to cubemap
  {
    std::array<rhi::TextureHandle, 1> hdriTextureData{hdriTexture};
    mDescriptorGenerateCubemap.write(0, hdriTextureData,
                                     rhi::DescriptorType::CombinedImageSampler);

    std::array<rhi::TextureHandle, 1> unfilteredCubemapData{unfilteredCubemap};
    mDescriptorGenerateCubemap.write(1, unfilteredCubemapData,
                                     rhi::DescriptorType::StorageImage);

    auto commandList = device->requestImmediateCommandList();

    std::array<rhi::MemoryBarrier, 1> memoryBarriers{
        {rhi::MemoryBarrier{rhi::Access::None, rhi::Access::ShaderWrite}}};
    std::array<rhi::ImageBarrier, 1> imageBarriers{
        {rhi::ImageBarrier{rhi::Access::None, rhi::Access::ShaderWrite,
                           rhi::ImageLayout::Undefined,
                           rhi::ImageLayout::General, unfilteredCubemap}}};

    commandList.pipelineBarrier(rhi::PipelineStage::PipeTop,
                                rhi::PipelineStage::ComputeShader,
                                memoryBarriers, imageBarriers);

    commandList.bindPipeline(mPipelineGenerateCubemap);
    commandList.bindDescriptor(mPipelineGenerateCubemap, 0,
                               mDescriptorGenerateCubemap);
    commandList.dispatch(GroupCount, GroupCount, CubemapSides);

    device->submitImmediate(commandList);
  }

  TextureUtils::generateMipMapsForTexture(
      device, unfilteredCubemap, rhi::ImageLayout::General, CubemapSides,
      numLevels, CubemapResolution, CubemapResolution);

  return CubemapData{unfilteredCubemap, levels};
}

Result<TextureAssetHandle>
HDRIImporter::generateIrradianceMap(const CubemapData &unfilteredCubemap,
                                    const Path &path) {
  auto *device = mRenderStorage.getDevice();

  const uint32_t GroupCount = unfilteredCubemap.levels.at(0).width / 32;

  rhi::TextureDescription cubemapDesc;
  cubemapDesc.type = rhi::TextureType::Cubemap;
  cubemapDesc.format = TargetFormat;
  cubemapDesc.height = unfilteredCubemap.levels.at(0).width;
  cubemapDesc.width = unfilteredCubemap.levels.at(0).width;
  cubemapDesc.layers = CubemapSides;
  cubemapDesc.levels = 1;
  cubemapDesc.usage = rhi::TextureUsage::Color | rhi::TextureUsage::Storage |
                      rhi::TextureUsage::Sampled |
                      rhi::TextureUsage::TransferSource;
  auto irradianceCubemap = mRenderStorage.createTexture(cubemapDesc);

  std::array<rhi::TextureHandle, 1> unfilteredCubemapData{
      unfilteredCubemap.texture};
  mDescriptorGenerateCubemap.write(0, unfilteredCubemapData,
                                   rhi::DescriptorType::CombinedImageSampler);

  std::array<rhi::TextureHandle, 1> irradianceCubemapData{irradianceCubemap};
  mDescriptorGenerateCubemap.write(1, irradianceCubemapData,
                                   rhi::DescriptorType::StorageImage);

  auto commandList = device->requestImmediateCommandList();

  std::array<rhi::MemoryBarrier, 2> memoryBarriers{
      rhi::MemoryBarrier{rhi::Access::None, rhi::Access::ShaderRead},
      rhi::MemoryBarrier{rhi::Access::None, rhi::Access::ShaderWrite}};

  std::array<rhi::ImageBarrier, 2> imageBarriers{
      rhi::ImageBarrier{
          rhi::Access::None, rhi::Access::ShaderRead, rhi::ImageLayout::General,
          rhi::ImageLayout::ShaderReadOnlyOptimal, unfilteredCubemap.texture},
      rhi::ImageBarrier{rhi::Access::None, rhi::Access::ShaderWrite,
                        rhi::ImageLayout::Undefined, rhi::ImageLayout::General,
                        irradianceCubemap}};

  commandList.pipelineBarrier(rhi::PipelineStage::PipeTop,
                              rhi::PipelineStage::ComputeShader, memoryBarriers,
                              imageBarriers);

  commandList.bindPipeline(mPipelineGenerateIrradianceMap);
  commandList.bindDescriptor(mPipelineGenerateIrradianceMap, 0,
                             mDescriptorGenerateCubemap);
  commandList.dispatch(GroupCount, GroupCount, CubemapSides);

  device->submitImmediate(commandList);

  auto levels = {unfilteredCubemap.levels.at(0)};

  AssetData<TextureAsset> asset{};
  asset.name = path.string();
  asset.size = TextureUtils::getBufferSizeFromLevels(levels);
  asset.data.type = TextureAssetType::Cubemap;
  asset.data.width = unfilteredCubemap.levels.at(0).width;
  asset.data.height = unfilteredCubemap.levels.at(0).height;
  asset.data.layers = 1;
  asset.data.format = TargetFormat;
  asset.data.levels = levels;

  asset.data.data.resize(asset.size);

  TextureUtils::copyTextureToData(
      device, irradianceCubemap, rhi::ImageLayout::ShaderReadOnlyOptimal,
      CubemapSides, asset.data.levels, asset.data.data.data());
  device->destroyTexture(irradianceCubemap);

  auto createdFileRes = mAssetCache.createTextureFromAsset(asset);
  if (createdFileRes.hasError()) {
    return Result<TextureAssetHandle>::Error(createdFileRes.getError());
  }

  return mAssetCache.loadTextureFromFile(createdFileRes.getData());
}

Result<TextureAssetHandle>
HDRIImporter::generateSpecularMap(const CubemapData &unfilteredCubemap,
                                  const Path &path) {
  auto *device = mRenderStorage.getDevice();

  rhi::TextureDescription cubemapDesc;
  cubemapDesc.type = rhi::TextureType::Cubemap;
  cubemapDesc.format = TargetFormat;
  cubemapDesc.height = unfilteredCubemap.levels.at(0).width;
  cubemapDesc.width = unfilteredCubemap.levels.at(0).width;
  cubemapDesc.layers = CubemapSides;
  cubemapDesc.levels = static_cast<uint32_t>(unfilteredCubemap.levels.size());
  cubemapDesc.usage = rhi::TextureUsage::Color | rhi::TextureUsage::Storage |
                      rhi::TextureUsage::Sampled |
                      rhi::TextureUsage::TransferSource;

  auto specularCubemap = mRenderStorage.createTexture(cubemapDesc);

  std::vector<rhi::TextureViewHandle> textureViews(
      unfilteredCubemap.levels.size());
  for (size_t level = 0; level < unfilteredCubemap.levels.size(); ++level) {
    rhi::TextureViewDescription description{};
    description.texture = specularCubemap;
    description.level = static_cast<uint32_t>(level);
    description.layerCount = CubemapSides;

    textureViews.at(level) = device->createTextureView(description);
  }

  std::array<rhi::TextureHandle, 1> unfilteredCubemapData{
      unfilteredCubemap.texture};
  mDescriptorGenerateCubemap.write(0, unfilteredCubemapData,
                                   rhi::DescriptorType::CombinedImageSampler);

  for (size_t mipLevel = 0; mipLevel < unfilteredCubemap.levels.size();
       ++mipLevel) {
    std::array<rhi::TextureViewHandle, 1> textureViewData{
        textureViews.at(mipLevel)};
    mDescriptorGenerateCubemap.write(1, textureViewData,
                                     rhi::DescriptorType::StorageImage);

    auto commandList = device->requestImmediateCommandList();
    commandList.bindPipeline(mPipelineGenerateSpecularMap);
    commandList.bindDescriptor(mPipelineGenerateSpecularMap, 0,
                               mDescriptorGenerateCubemap);

    auto &level = unfilteredCubemap.levels.at(mipLevel);
    uint32_t groupSize = level.width;
    float roughness = static_cast<float>(mipLevel) /
                      static_cast<float>(unfilteredCubemap.levels.size());

    glm::vec4 data{roughness, mipLevel, static_cast<float>(level.width), 0.0f};
    commandList.pushConstants(mPipelineGenerateSpecularMap,
                              rhi::ShaderStage::Compute, 0, sizeof(glm::vec4),
                              glm::value_ptr(data));
    commandList.dispatch(groupSize, groupSize, CubemapSides);
    device->submitImmediate(commandList);
  }

  for (auto view : textureViews) {
    device->destroyTextureView(view);
  }

  AssetData<TextureAsset> asset{};
  asset.name = path.string();
  asset.size = TextureUtils::getBufferSizeFromLevels(unfilteredCubemap.levels);
  asset.data.type = TextureAssetType::Cubemap;
  asset.data.width = unfilteredCubemap.levels.at(0).width;
  asset.data.height = unfilteredCubemap.levels.at(0).height;
  asset.data.layers = 1;
  asset.data.format = TargetFormat;
  asset.data.levels = unfilteredCubemap.levels;

  asset.data.data.resize(asset.size);

  TextureUtils::copyTextureToData(
      device, specularCubemap, rhi::ImageLayout::ShaderReadOnlyOptimal,
      CubemapSides, asset.data.levels, asset.data.data.data());
  device->destroyTexture(specularCubemap);

  auto createdFileRes = mAssetCache.createTextureFromAsset(asset);
  if (createdFileRes.hasError()) {
    return Result<TextureAssetHandle>::Error(createdFileRes.getError());
  }

  return mAssetCache.loadTextureFromFile(createdFileRes.getData());
}

} // namespace liquid::editor
