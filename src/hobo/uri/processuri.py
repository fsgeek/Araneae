import os
import pandas as pd
import sys
import argparse

def generate_header(df, header):
    guard = '__{}__'.format(header.upper().replace('.', '_'))
    with open(header, 'wt') as fd:
        fd.write('// *** AUTOMATICALLY GENERATED  ***\n')
        fd.write('// *** EDITS MAY BE OVERWRITTEN ***\n')
        fd.write('// Download CSV file from IANA: https://www.iana.org/assignments/uri-schemes/uri-schemes.xml\n')
        fd.write('\n'*2)
        fd.write('#pragma once\n\n')
        fd.write('#if !defined({})\n'.format(guard))
        fd.write('#define {} (1)\n'.format(guard))
        fd.write('\n'*2)
        fd.write('typedef struct uri_schemes {\n')
        fd.write('\tconst char *scheme_name;\n')
        fd.write('\tconst char *description;\n')
        fd.write('\tconst char *reference;\n')
        fd.write('} uri_schemes_t;\n')
        fd.write('\n'*2)
        fd.write('extern uri_schemes_t uri_scheme_table[];\n\n')

        fd.write('#endif // {}\n'.format(guard))
    print('Generated header file {}'.format(header))

def generate_code(df, code, header):
    if '/' in header: header = os.path.basename(header)
    with open(code, 'wt') as fd:
        fd.write('// *** AUTOMATICALLY GENERATED  ***\n')
        fd.write('// *** EDITS MAY BE OVERWRITTEN ***\n')
        fd.write('// Download CSV file from IANA: https://www.iana.org/assignments/uri-schemes/uri-schemes.xml\n')
        fd.write('\n'*2)
        fd.write('#include "{}"\n'.format(header))
        fd.write('\n'*2)
        fd.write('uri_schemes_t uri_scheme_table[] = {\n')
        fd.write('\t//{:38} {:<38} {}\n'.format('Scheme', 'Description', 'Reference'))
        for index, row in df.iterrows():
            fd.write('\t{')
            fd.write('{:<38} {:<38} {}'.format('"{}",'.format(row['URI Scheme'].strip()), 
                                               '"{}",'.format(row['Description'][:34].strip()),
                                               '"{}"'.format(row['Reference'][:34].strip())))
            fd.write('},\n')
        fd.write('};\n\n')


def main():
    print(os.getcwd())
    print(os.listdir('.'))
    csv_files = [c for c in os.listdir('.') if c.endswith('.csv')]   
    parser = argparse.ArgumentParser(description='Generate environmental search data for testing.')
    if len(csv_files) is 1: parser.add_argument('--urischemes', dest='urifile', default=csv_files[0], type=str, help='URI scheme file to process in CSV format')
    else: parser.add_argument('--urischemes', dest='urischemes', required=True, type=str, help='URI scheme file to process in CSV format')
    parser.add_argument('--header', dest='header', default='urischemes.h', help='header file to generate')
    parser.add_argument('--code', dest='code', default='urischemes.c', help='C code file to generate')
    parser.add_argument('--useall', dest='useall', default=False, action='store_true', help='Encode all URIs, not just permanent ones')
    args = parser.parse_args()

    assert os.path.exists(args.urifile), 'Specified URI Scheme file does not exist'
    print('processing URI scheme file {}'.format(args.urifile))

    df = pd.read_csv(args.urifile)
    if not args.useall: df = df.loc[df['Status'] == 'Permanent']
    #print('loaded dataframe {}'.format(df))
    #for index, row in df.iterrows(): print('scheme = {}, description = {}, reference = {}'.format(row['URI Scheme'], row['Description'][:40], row['Reference']))

    generate_header(df, args.header)
    generate_code(df, args.code, args.header)
    return

if __name__ == "__main__":
    main()
