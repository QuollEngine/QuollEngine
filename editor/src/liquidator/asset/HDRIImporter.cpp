#include "liquid/core/Base.h"
#include "liquid/renderer/RenderGraph.h"
#include "liquid/renderer/TextureUtils.h"

#include "HDRIImporter.h"

#include <stb/stb_image.h>

namespace liquid::editor {

static constexpr uint32_t CubemapSides = 6;

static constexpr rhi::Format TargetFormat = rhi::Format::Rgba16Float;
static constexpr uint32_t FormatSize = 4 * 2;

HDRIImporter::HDRIImporter(AssetCache &assetCache, rhi::RenderDevice *device)
    : mAssetCache(assetCache), mDevice(device) {

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

    auto layout = mDevice->createDescriptorLayout({{binding0, binding1}});
    mDescriptorGenerateCubemap = mDevice->createDescriptor(layout);
  }

  {
    auto shader = mDevice->createShader(
        {shadersPath / "equirectangular-to-cubemap.comp.spv"});
    mPipelineGenerateCubemap =
        mDevice->createPipeline(rhi::ComputePipelineDescription{shader});
  }

  {
    auto shader = mDevice->createShader(
        {shadersPath / "generate-irradiance-map.comp.spv"});
    mPipelineGenerateIrradianceMap =
        mDevice->createPipeline(rhi::ComputePipelineDescription{shader});
  }

  {
    auto shader =
        mDevice->createShader({shadersPath / "generate-specular-map.comp.spv"});
    mPipelineGenerateSpecularMap =
        mDevice->createPipeline(rhi::ComputePipelineDescription{shader});
  }

  {
    auto shader =
        mDevice->createShader({shadersPath / "generate-brdf-lut.comp.spv"});
    mPipelineGenerateBrdfLut =
        mDevice->createPipeline(rhi::ComputePipelineDescription{shader});
  }
}

Result<Path> HDRIImporter::loadFromPath(const Path &originalAssetPath,
                                        const Path &engineAssetPath) {
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
    mDevice->destroyTexture(unfilteredCubemap.texture);
    std::filesystem::remove_all(engineAssetPath);

    return Result<Path>::Error(irradianceCubemap.getError());
  }

  auto specularMapName = originalAssetPath.stem().string() + "-specular";
  auto specularCubemap =
      generateSpecularMap(unfilteredCubemap, relPath / specularMapName);

  if (specularCubemap.hasError()) {
    mDevice->destroyTexture(unfilteredCubemap.texture);
    mAssetCache.getRegistry().getTextures().deleteAsset(
        irradianceCubemap.getData());
    std::filesystem::remove_all(engineAssetPath);

    return Result<Path>::Error(specularCubemap.getError());
  }

  auto brdfLutName = originalAssetPath.stem().string() + "-brdf-lut";
  auto brdfLut = generateBrdfLut(unfilteredCubemap, relPath / brdfLutName);

  if (brdfLut.hasError()) {
    mDevice->destroyTexture(unfilteredCubemap.texture);
    mAssetCache.getRegistry().getTextures().deleteAsset(
        irradianceCubemap.getData());
    mAssetCache.getRegistry().getTextures().deleteAsset(
        specularCubemap.getData());
    std::filesystem::remove_all(engineAssetPath);

    return Result<Path>::Error(brdfLut.getError());
  }

  mDevice->destroyTexture(unfilteredCubemap.texture);

  AssetData<EnvironmentAsset> environment{};
  environment.path =
      engineAssetPath / originalAssetPath.filename().replace_extension("lqenv");
  environment.data.brdfLut = brdfLut.getData();
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
      mDevice, data, hdriTexture, rhi::ImageLayout::ShaderReadOnlyOptimal, 1,
      {{0, static_cast<size_t>(width) * height * 4 * sizeof(float),
        static_cast<uint32_t>(width), static_cast<uint32_t>(height)}});

  return hdriTexture;
}

HDRIImporter::CubemapData
HDRIImporter::convertEquirectangularToCubemap(float *data, uint32_t width,
                                              uint32_t height) {
  rhi::TextureDescription hdriTextureDesc{};
  hdriTextureDesc.usage = rhi::TextureUsage::Color |
                          rhi::TextureUsage::TransferDestination |
                          rhi::TextureUsage::Sampled;
  hdriTextureDesc.format = rhi::Format::Rgba32Float;
  hdriTextureDesc.width = width;
  hdriTextureDesc.height = height;
  hdriTextureDesc.layers = 1;

  auto hdriTexture = mDevice->createTexture(hdriTextureDesc);

  TextureUtils::copyDataToTexture(
      mDevice, data, hdriTexture, rhi::ImageLayout::ShaderReadOnlyOptimal, 1,
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

  auto unfilteredCubemap = mDevice->createTexture(cubemapDesc);

  // Convert equirectangular texture to cubemap
  {
    mDescriptorGenerateCubemap.write(0, {hdriTexture},
                                     rhi::DescriptorType::CombinedImageSampler);
    mDescriptorGenerateCubemap.write(1, {unfilteredCubemap},
                                     rhi::DescriptorType::StorageImage);

    auto commandList = mDevice->requestImmediateCommandList();
    commandList.pipelineBarrier(
        rhi::PipelineStage::PipeTop, rhi::PipelineStage::ComputeShader,
        {rhi::MemoryBarrier{rhi::Access::None, rhi::Access::ShaderWrite}},
        {rhi::ImageBarrier{rhi::Access::None, rhi::Access::ShaderWrite,
                           rhi::ImageLayout::Undefined,
                           rhi::ImageLayout::General, unfilteredCubemap}});

    commandList.bindPipeline(mPipelineGenerateCubemap);
    commandList.bindDescriptor(mPipelineGenerateCubemap, 0,
                               mDescriptorGenerateCubemap);
    commandList.dispatch(GroupCount, GroupCount, CubemapSides);

    mDevice->submitImmediate(commandList);
  }

  TextureUtils::generateMipMapsForTexture(
      mDevice, unfilteredCubemap, rhi::ImageLayout::General, CubemapSides,
      numLevels, CubemapResolution, CubemapResolution);

  return CubemapData{unfilteredCubemap, levels};
}

Result<TextureAssetHandle>
HDRIImporter::generateIrradianceMap(const CubemapData &unfilteredCubemap,
                                    const Path &path) {
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
  auto irradianceCubemap = mDevice->createTexture(cubemapDesc);

  mDescriptorGenerateCubemap.write(0, {unfilteredCubemap.texture},
                                   rhi::DescriptorType::CombinedImageSampler);
  mDescriptorGenerateCubemap.write(1, {irradianceCubemap},
                                   rhi::DescriptorType::StorageImage);

  auto commandList = mDevice->requestImmediateCommandList();
  commandList.pipelineBarrier(
      rhi::PipelineStage::PipeTop, rhi::PipelineStage::ComputeShader,
      {rhi::MemoryBarrier{rhi::Access::None, rhi::Access::ShaderRead},
       rhi::MemoryBarrier{rhi::Access::None, rhi::Access::ShaderWrite}},
      {rhi::ImageBarrier{rhi::Access::None, rhi::Access::ShaderRead,
                         rhi::ImageLayout::General,
                         rhi::ImageLayout::ShaderReadOnlyOptimal,
                         unfilteredCubemap.texture},
       rhi::ImageBarrier{rhi::Access::None, rhi::Access::ShaderWrite,
                         rhi::ImageLayout::Undefined, rhi::ImageLayout::General,
                         irradianceCubemap}});

  commandList.bindPipeline(mPipelineGenerateIrradianceMap);
  commandList.bindDescriptor(mPipelineGenerateIrradianceMap, 0,
                             mDescriptorGenerateCubemap);
  commandList.dispatch(GroupCount, GroupCount, CubemapSides);

  mDevice->submitImmediate(commandList);

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

  asset.data.data = new char[asset.size];

  TextureUtils::copyTextureToData(
      mDevice, irradianceCubemap, rhi::ImageLayout::ShaderReadOnlyOptimal,
      CubemapSides, asset.data.levels, asset.data.data);
  mDevice->destroyTexture(irradianceCubemap);

  auto createdFileRes = mAssetCache.createTextureFromAsset(asset);
  if (createdFileRes.hasError()) {
    return Result<TextureAssetHandle>::Error(createdFileRes.getError());
  }

  return mAssetCache.loadTextureFromFile(createdFileRes.getData());
}

Result<TextureAssetHandle>
HDRIImporter::generateSpecularMap(const CubemapData &unfilteredCubemap,
                                  const Path &path) {
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

  auto specularCubemap = mDevice->createTexture(cubemapDesc);

  std::vector<rhi::TextureViewHandle> textureViews(
      unfilteredCubemap.levels.size());
  for (size_t level = 0; level < unfilteredCubemap.levels.size(); ++level) {
    rhi::TextureViewDescription description{};
    description.texture = specularCubemap;
    description.level = static_cast<uint32_t>(level);
    description.layerCount = CubemapSides;

    textureViews.at(level) = mDevice->createTextureView(description);
  }

  mDescriptorGenerateCubemap.write(0, {unfilteredCubemap.texture},
                                   rhi::DescriptorType::CombinedImageSampler);

  for (size_t mipLevel = 0; mipLevel < unfilteredCubemap.levels.size();
       ++mipLevel) {
    mDescriptorGenerateCubemap.write(1, {textureViews.at(mipLevel)},
                                     rhi::DescriptorType::StorageImage);

    auto commandList = mDevice->requestImmediateCommandList();
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
    mDevice->submitImmediate(commandList);
  }

  for (auto view : textureViews) {
    mDevice->destroyTextureView(view);
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

  asset.data.data = new char[asset.size];

  TextureUtils::copyTextureToData(
      mDevice, specularCubemap, rhi::ImageLayout::ShaderReadOnlyOptimal,
      CubemapSides, asset.data.levels, asset.data.data);
  mDevice->destroyTexture(specularCubemap);

  auto createdFileRes = mAssetCache.createTextureFromAsset(asset);
  if (createdFileRes.hasError()) {
    return Result<TextureAssetHandle>::Error(createdFileRes.getError());
  }

  return mAssetCache.loadTextureFromFile(createdFileRes.getData());
}

Result<TextureAssetHandle>
HDRIImporter::generateBrdfLut(const CubemapData &unfilteredCubemap,
                              const Path &path) {
  static constexpr uint32_t GroupSize = 16;

  rhi::TextureDescription textureDesc;
  textureDesc.type = rhi::TextureType::Standard;
  textureDesc.format = TargetFormat;
  textureDesc.height = unfilteredCubemap.levels.at(0).width;
  textureDesc.width = unfilteredCubemap.levels.at(0).width;
  textureDesc.layers = 1;
  textureDesc.levels = 1;
  textureDesc.usage = rhi::TextureUsage::Color | rhi::TextureUsage::Storage |
                      rhi::TextureUsage::Sampled |
                      rhi::TextureUsage::TransferSource;

  auto brdfLut = mDevice->createTexture(textureDesc);

  mDescriptorGenerateCubemap.write(0, {unfilteredCubemap.texture},
                                   rhi::DescriptorType::CombinedImageSampler);
  mDescriptorGenerateCubemap.write(1, {brdfLut},
                                   rhi::DescriptorType::StorageImage);

  auto commandList = mDevice->requestImmediateCommandList();
  commandList.bindPipeline(mPipelineGenerateBrdfLut);
  commandList.bindDescriptor(mPipelineGenerateBrdfLut, 0,
                             mDescriptorGenerateCubemap);
  commandList.dispatch(textureDesc.width / GroupSize,
                       textureDesc.height / GroupSize, 1);
  mDevice->submitImmediate(commandList);

  auto levels = std::vector{TextureAssetLevel{
      0,
      static_cast<size_t>(textureDesc.width) * textureDesc.height * FormatSize,
      textureDesc.width, textureDesc.height}};

  AssetData<TextureAsset> asset{};
  asset.name = path.string();
  asset.size = TextureUtils::getBufferSizeFromLevels(levels);
  asset.data.type = TextureAssetType::Standard;
  asset.data.width = levels.at(0).width;
  asset.data.height = levels.at(0).height;
  asset.data.layers = 1;
  asset.data.format = TargetFormat;
  asset.data.levels = levels;

  asset.data.data = new char[asset.size];

  TextureUtils::copyTextureToData(
      mDevice, brdfLut, rhi::ImageLayout::ShaderReadOnlyOptimal,
      asset.data.layers, asset.data.levels, asset.data.data);
  mDevice->destroyTexture(brdfLut);

  auto createdFileRes = mAssetCache.createTextureFromAsset(asset);
  if (createdFileRes.hasError()) {
    return Result<TextureAssetHandle>::Error(createdFileRes.getError());
  }

  return mAssetCache.loadTextureFromFile(createdFileRes.getData());
}

} // namespace liquid::editor
