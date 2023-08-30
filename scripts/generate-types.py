from pathlib import Path
import os
import re

FORMATS_PATH = Path(os.path.dirname(__file__), '..', 'formats.txt')
FORMAT_PATTERN = '([A-Za-z]+)(\d+)([A-Za-z]+)'

formats = []

with open(FORMATS_PATH, 'r', encoding = 'utf-8') as f:
    contents = f.read()
    lines = contents.splitlines()
    for line in lines:
        matches = re.findall(FORMAT_PATTERN, line)
        if len(matches) == 0:
            continue
        segments = []
        for (component, bits, data_type) in matches:
            print(component, bits, data_type)
            segments.append((component, bits, data_type))
        formats.append(segments)

def output_enum(formats):
    ENUM_TEMPLATE = '''
    enum Format {
    Undefined = 0,
    $enum_values
    };'''
    enum_values = ',\n'.join(formats)
    return ENUM_TEMPLATE.replace('$enum_values', enum_values)


def output_vulkan_map(formats):
    RHI_TO_VK_CASE_TEMPLATE = '''
    case $original:
        return $vulkan;'''
    RHI_TO_VK_DEFAULT_TEMPLATE = '''
    case rhi::Format::Undefined:
    default:
        QuollAssert(false, "Undefined format");
        return VK_FORMAT_UNDEFINED;'''

    VK_TO_RHI_CASE_TEMPLATE = '''
    case $vulkan:
        return $original;'''
    VK_TO_RHI_DEFAULT_TEMPLATE = '''
    case VK_FORMAT_UNDEFINED:
    default:
        QuollAssert(false, "Undefined format");
        return rhi::Format::Undefined;
    '''

    TYPE_MAP = {
        'Float': 'SFLOAT',
    }        

    rhi_to_vk = []
    vk_to_rhi = []
    original_formats = []
    for segments in formats:
        original = ''
        vk_segments = []
        for (components, bits_per_component, data_type) in segments:
            original = original + components + bits_per_component + data_type
            if components == 'Depth':
                vulkan_components = f'D{bits_per_component}'
            elif components == 'Stencil':
                vulkan_components = f'S{bits_per_component}'
            else:
                vulkan_components = ''.join([f'{x}{bits_per_component}' for x in components])
            vulkan_type = TYPE_MAP.get(data_type) or data_type
            vk_segments.append(f'{vulkan_components}_{vulkan_type}')

        rhi_format = f'rhi::Format::{original}'
        vk_format = f'VK_FORMAT_{"_".join(vk_segments)}'.upper()

        rhi_to_vk.append(RHI_TO_VK_CASE_TEMPLATE.replace('$original', rhi_format).replace('$vulkan', vk_format))
        vk_to_rhi.append(VK_TO_RHI_CASE_TEMPLATE.replace('$original', rhi_format).replace('$vulkan', vk_format))
        original_formats.append(original)

    rhi_to_vk.append(RHI_TO_VK_DEFAULT_TEMPLATE)
    vk_to_rhi.append(VK_TO_RHI_DEFAULT_TEMPLATE)

    return (''.join(rhi_to_vk), ''.join(vk_to_rhi), original_formats)

rhi_to_vk, vk_to_rhi, original_formats = output_vulkan_map(formats)

print('VkFormat to RHI::Format')
print(rhi_to_vk, '\n')
print('RHI::Format to VkFormat')
print(vk_to_rhi, '\n')

enum_values = output_enum(original_formats)
print('RHI::Format enum')
print(enum_values, '\n')
