#! /usr/bin/env python

from os import path
import sys
import subprocess
import argparse
import io
import platform

VERSION = '1.4.1'


def is_valid_file(parser, arg):
  if not path.isfile(arg):
    parser.error("the file %s does not exist!"%arg)
  else:
    return open(arg, 'r')


def llvm2bplParser():
  parser = argparse.ArgumentParser(add_help=False)
  parser.add_argument('-v', '--version', action='version', version='SMACK version ' + VERSION)
  parser.add_argument('infile', metavar='<file>',
                      type=lambda x: is_valid_file(parser,x),
                      help='input LLVM file')
  parser.add_argument('-o', '--output', dest='outfile', metavar='<file>', default='a.bpl',
                      type=argparse.FileType('w'),
                      help='output Boogie file (default: %(default)s)')
  parser.add_argument('-d', '--debug', dest='debug', action="store_true", default=False,
                      help='turn on debug info')
  return parser


def llvm2bpl(infile, debugFlag, memImpls):
    
  cmd = ['smack', '-source-loc-syms', infile.name]
  #cmd = ['smack', infile.name]
  if debugFlag: cmd.append('-debug')
  if memImpls: cmd.append('-mem-mod-impls')
  print >> sys.stderr, cmd
  p = subprocess.Popen(cmd)

  p.wait()
  if p.returncode != 0:
    print >> sys.stderr, "SMACK encountered an error:"
    print >> sys.stderr, output[0:1000], "... (output truncated)"
    sys.exit("SMACK returned exit status %s" % p.returncode)

  with open('a.bpl', 'r') as outputFile:
    output = outputFile.read()

  # bplStartIndex = output.find('// SMACK-PRELUDE-BEGIN')
  # bpl = output[bplStartIndex:]
  # return bpl
  return output
 

if __name__ == '__main__':

  # parse command line arguments
  parser = argparse.ArgumentParser(description='Outputs a plain Boogie file generated from the input LLVM file.', parents=[llvm2bplParser()])
  args = parser.parse_args()

  bpl = llvm2bpl(args.infile, args.debug, args.memmod, args.memimpls)

  # write final output
  args.outfile.write(bpl)
  args.outfile.close()

