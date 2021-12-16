#!/usr/bin/env python

# Do not execute this file directly. Using blusher script instead.
import sys
import hashlib
from argparse import ArgumentParser
from pathlib import Path

#==================
# RCC Utils
#==================
class Brc:
    def __init__(self, path):
        self._path = path

    def brc_dir(self):
        p = Path(self._path)

        return p.parent.resolve()

    def md5_self(self):
        f = open(self._path, 'rb')
        data = f.read()
        f.close()

        return hashlib.md5(data).hexdigest()

    def header_guard_begin(self):
        md5 = self.md5_self()
        macro = '#ifndef BRC_' + md5 + '\n'
        macro += '#define BRC_' + md5 + '\n'

        return macro

    def header_guard_end(self):
        return '#endif\n'

    def header_decl(self):
        md5 = self.md5_self()
        return f'void bl_register_resource_{md5}();\n'

    def header_macro(self):
        md5 = self.md5_self()
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
    print(brc.make_header())

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
