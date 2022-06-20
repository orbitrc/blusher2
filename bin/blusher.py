#!/usr/bin/env python

# Do not execute this file directly. Using blusher script instead.
import os
import sys
import hashlib
from argparse import ArgumentParser
from pathlib import Path
import xml.etree.ElementTree as ET

VERSION_MAJOR = 1
VERSION_MINOR = 0
VERSION_PATCH = 0
VERSION = f'{VERSION_MAJOR}.{VERSION_MINOR}.{VERSION_PATCH}'

#==================
# RCC Utils
#==================
class Brc:
    class Resource:
        class File:
            def __init__(self, filename, alias):
                self._filename = filename
                self._alias = alias

            def filename(self):
                return self._filename

            def filename_or_alias(self):
                if self._alias is not None:
                    return self._alias
                return self._filename

        def __init__(self, prefix):
            self._prefix = prefix
            self._files = []

        def add_file(self, filename, alias=None):
            file = Brc.Resource.File(filename, alias)
            self._files.append(file)

        def prefix(self):
            return self._prefix

    def __init__(self, path):
        """Create Brc object.

        Keyword arguments:

        path -- the absolute path
        """
        self._path = path
        # Output file name.
        self._name = path.name.replace('.brc', '')
        self._md5 = self.md5_self()
        self._resources = []

    def brc_dir(self):
        """The directory which .brc file located."""
        p = Path(self._path)

        return str(p.parent.resolve())

    def name(self):
        """Name of .brc file. Without .brc extension."""
        return self._name

    def md5_self(self):
        f = open(self._path, 'rb')
        data = f.read()
        f.close()

        return hashlib.md5(data).hexdigest()

    def parse(self):
        f = open(self._path, 'r')
        data = f.read()
        f.close()

        try:
            root = ET.fromstring(data)
        except ET.ParseError:
            print('blusher rcc: Parse error!')
            exit(1)

        if root.tag != 'brc':
            print('blusher rcc: File not starts with <brc> tag.')
            exit(1)

        for resource in root:
            if resource.tag != 'resource':
                print('blusher rcc: Invalid tag ' + f'<{resource.tag}>')
                exit(1)
            prefix = resource.attrib.get('prefix')
            resource_obj = Brc.Resource(prefix)
            for file in resource:
                alias = file.attrib.get('alias', None)
                resource_obj.add_file(file.text, alias)
            self._resources.append(resource_obj)


    def header_guard_begin(self):
        md5 = self._md5
        macro = '#ifndef BRC_' + md5 + '\n'
        macro += '#define BRC_' + md5 + '\n'

        return macro

    def header_guard_end(self):
        return '#endif\n'

    def header_decl(self):
        md5 = self._md5
        return f'void bl_register_resource_{md5}();\n'

    def header_macro(self):
        md5 = self._md5
        macro = '#ifdef BL_REGISTER_RESOURCE\n'
        macro += '    #undef BL_REGISTER_RESOURCE\n'
        macro += '#endif\n'
        macro += '#define BL_REGISTER_RESOURCE() \\\n'
        macro += f'    bl_register_resource_{md5}();\n'

        return macro

    def make_header(self):
        header = self.header_guard_begin()
        header += '\n'
        header += self.header_decl()
        header += '\n'
        header += self.header_macro()
        header += '\n'
        header += self.header_guard_end()

        return header

    def source_comment(self):
        comment = '/*==============================================*/\n'
        comment += f'/* Blusher Resource Compiler Version {VERSION}      */\n'
        comment += '/* DO NOT MODIFY THIS FILE MANUALLY!            */\n'
        comment += '/*==============================================*/\n'

        return comment

    def source_raw_data(self):
        index = 0
        raw_data = ''

        for resource in self._resources:
            for file in resource._files:
                # Get resource path for file.
                path = resource.prefix() + '/' + file.filename_or_alias()
                raw_data += '/* ' + path + ' */\n'
                raw_data += f'static unsigned char data_{index}[] = {{\n'

                abs_path = self.brc_dir() + '/' + file.filename()
                f = open(abs_path, 'rb')
                data = f.read()
                f.close()
                i = 0
                raw_data += '    '
                for b in data:
                    raw_data += hex(b) + ','
                    if i < 8:
                        raw_data += ' '
                    if i == 7:
                        raw_data += '\n'
                        raw_data += '    '
                        i = 0
                        continue
                    i += 1

                raw_data = raw_data.rstrip()
                raw_data += '\n};\n\n'
                index += 1

        return raw_data

    def source_resource_data(self):
        index = 0
        resource_data = ''

        for resource in self._resources:
            for file in resource._files:
                # Get resource path for file.
                path = resource.prefix() + '/' + file.filename_or_alias()
                resource_data += f'static bl::Resource::Data resource_data_{index}(\n'
                resource_data += f'    "{path}",\n'
                resource_data += f'    data_{index},\n'
                resource_data += f'    sizeof(data_{index})\n'
                resource_data += ');\n\n'
                index += 1

        return resource_data

    def source_register_function(self):
        index = 0
        register_function = f'void bl_register_resource_{self._md5}()\n{{\n'

        for resource in self._resources:
            for file in resource._files:
                path = resource.prefix() + '/' + file.filename_or_alias()
                register_function += f'    // {path}\n'
                register_function += f'    bl::app->resource()->add_data(&resource_data_{index});\n'
                index += 1

        register_function += '}\n'

        return register_function

    def make_source(self):
        source = self.source_comment()
        source += f'#include "{self._name}.h"\n'
        source += '\n'
        source += '#include <blusher/application.h>\n'
        source += '#include <blusher/resource.h>\n'
        source += '\n'
        source += self.source_raw_data()
        source += self.source_resource_data()
        source += self.source_register_function()

        return source

#===============
# RCC
#===============
def rcc(input_file, output=None):
    if not input_file.exists():
        print(f'blusher rcc: File {input_file} does not exist.')
        exit(1)
    # Get absolute path.
    abs_path = input_file.resolve()
    brc = Brc(abs_path)
    brc_md5 = brc.md5_self()
    print(abs_path)
    print(brc.brc_dir())
    print(brc.header_guard_begin())
    print(output)
    brc.parse()

    # Create C header file.
    f = open(brc.brc_dir() + '/' + brc.name() + '.h', 'w')
    f.write(brc.make_header())
    f.close()

    # Create C source file.
    f = open(brc.brc_dir() + '/' + brc.name() + '.cpp', 'w')
    f.write(brc.make_source())
    f.close()

    # Compile C source file and create a static library.
    cflags = ' -fPIC '
    if not os.path.exists('/usr/include/blusher/application.h'):
        blusher_bin_dir = os.path.dirname(os.path.realpath(__file__))
        cflags += f' -I{blusher_bin_dir}/../include '
    cpp_path = brc.brc_dir() + '/' + f'{brc.name()}.cpp'
    o_path = brc.brc_dir() + '/' + f'{brc.name()}.o'
    a_path = brc.brc_dir() + '/' + f'lib{brc.name()}.a'
    cmd = f'g++ {cflags} -c {cpp_path} -o {o_path}'
    print(f'blusher rcc: {cmd}')
    os.system(cmd)
    cmd = f'ar -rcs {a_path} {o_path}'
    print(f'blusher rcc: {cmd}')
    os.system(cmd)

if __name__ == '__main__':
    argp = ArgumentParser(
        prog='blusher',
        description='Blusher CLI tool.'
    )
    sub_argp = argp.add_subparsers(dest='subcommand', help='Sub-commands')
    # Sub-command 'rcc'
    argp_rcc = sub_argp.add_parser('rcc',
        help='Blusher Resource Compiler.')
    argp_rcc.add_argument('input', type=Path, help='Input .brc file.')
    argp_rcc.add_argument('--output', '-o', type=Path,
        help='Output file instead stdout.')

    args = argp.parse_args()

    if args.subcommand == 'rcc':
        rcc(args.input, args.output)
