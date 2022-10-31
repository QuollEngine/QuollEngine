from pathlib import Path
import os
import re

FORMATS_PATH = Path(os.path.dirname(__file__), '..', 'formats.txt')
FORMAT_PATTERN = '^([A-Za-z]+)(\d+)(\S+)$'

formats = []

with open(FORMATS_PATH, 'r', encoding = 'utf-8') as f:
    contents = f.read()
    lines = contents.splitlines()
    for line in lines:
        matches = re.search(FORMAT_PATTERN, line)
        if matches is not None:
            formats.append((line, matches.group(1), matches.group(2), matches.group(3)))

def output_enum(formats):
    ENUM_TEMPLATE = '''
    enum Format {
    Undefined = 0,
    $enum_values
    };'''
    originals = [x[0] for x in formats]
    enum_values = ',\n'.join(originals)
    return ENUM_TEMPLATE.replace('$enum_values', enum_values)


def output_vulkan_map(formats):
    RHI_TO_VK_CASE_TEMPLATE = '''
    case $original:
        return $vulkan;'''
    RHI_TO_VK_DEFAULT_TEMPLATE = '''
    case rhi::Format::Undefined:
    default:
        LIQUID_ASSERT(false, "Undefined format");
        return VK_FORMAT_UNDEFINED;'''

    VK_TO_RHI_CASE_TEMPLATE = '''
    case $vulkan:
        return $original;'''
    VK_TO_RHI_DEFAULT_TEMPLATE = '''
    case VK_FORMAT_UNDEFINED:
    default:
        LIQUID_ASSERT(false, "Undefined format");
        return rhi::Format::Undefined;
    '''

    TYPE_MAP = {
        'Float': 'SFLOAT',
    }        

    rhi_to_vk = []
    vk_to_rhi = []
    for original, components, bits_per_component, data_type in formats:
        if components == 'Depth':
            vulkan_components = f'D{bits_per_component}'
        else:
            vulkan_components = ''.join([f'{x}{bits_per_component}' for x in components])

        vulkan_type = TYPE_MAP.get(data_type) or data_type

        rhi_format = f'rhi::Format::{original}'
        vk_format = f'VK_FORMAT_{vulkan_components}_{vulkan_type}'.upper()

        rhi_to_vk.append(RHI_TO_VK_CASE_TEMPLATE.replace('$original', rhi_format).replace('$vulkan', vk_format))
        vk_to_rhi.append(VK_TO_RHI_CASE_TEMPLATE.replace('$original', rhi_format).replace('$vulkan', vk_format))

    rhi_to_vk.append(RHI_TO_VK_DEFAULT_TEMPLATE)
    vk_to_rhi.append(VK_TO_RHI_DEFAULT_TEMPLATE)

    return (''.join(rhi_to_vk), ''.join(vk_to_rhi))



rhi_to_vk, vk_to_rhi = output_vulkan_map(formats)
print('VkFormat to RHI::Format')
print(rhi_to_vk, '\n')
print('RHI::Format to VkFormat')
print(vk_to_rhi, '\n')

enum_values = output_enum(formats)
print('RHI::Format enum')
print(enum_values, '\n')
