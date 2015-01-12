#!/usr/bin/env python3
#-*- coding: utf-8 -*-
import sys
import glob

def main():
    if len(sys.argv) < 2:
        print('Error: Must specify header file path')
        print('usually in src/engine/sdosscripts.h')
        return

    out_file = sys.argv[1]

    def write(s=''):
        out.write(s + '\n')

    file_names = glob.iglob('scripts/*.cfg')
    script_names = []

    with open(out_file, 'w') as out:
        write('#ifndef sauerbraten_sdosscripts_h')
        write('#define sauerbraten_sdosscripts_h')
        write()
        write('/* automatically generated file -- do not edit */')
        write()

        for file_name in file_names:
            raw_name = file_name[0:file_name.index('.')].split('/')[1]
            script_names.append(raw_name)

            write('const char *script_{:s} ='.format(raw_name))

            with open(file_name, 'r') as cfg_file:
                for line in cfg_file:
                    s = line.rstrip().replace('\\', r'\\').replace('"', r'\"')
                    if s:
                        write(r'"' + s + r'\n"')

                write(';')
                write()
  
        if script_names:
            write('const char *sdos_scripts[] = {{ script_{:s}, 0 }};'.format(', script_'.join(script_names)))
        else:
            write('const char *sdos_scripts[] = {0};')

        write()
        write('#endif /* sauerbraten_sdosscripts_h */')
        write()

if __name__ == "__main__": 
    main()
