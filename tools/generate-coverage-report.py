#!/usr/bin/python

import os
import argparse
import subprocess

parser = argparse.ArgumentParser(description='Generate test coverage report.')

parser.add_argument('compiler', nargs='?', type=str,
                    choices=['clang', 'gnu'], default='gnu',
                    help='the compiler used to build the program (default: gnu)')

args = parser.parse_args()

root = os.path.dirname(__file__)
root = os.path.dirname(root)

output = os.path.join(root, 'docs')
output = os.path.join(output, 'coverage.html')

gcov = 'llvm-cov gcov' if args.compiler == 'clang' else 'gcov'

subprocess.run(['gcovr', '--gcov-executable', gcov, '--html-details',
                '-r', root, '-o', output,
                '--exclude', '.*_deps.*', '--exclude', 'tests.*'
                ])
