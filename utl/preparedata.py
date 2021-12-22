#!/usr/bin/python3

import sys
import getopt
import util

def main(argv):
    try:
        opts, args = getopt.getopt(argv,"hd:")
    except getopt.GetoptError:
        print('preparedata.py -d <data_directory>')
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print('preparedata.py -d <data_directory>')
            sys.exit()
        elif opt in ("-d"):
            data_directory = arg
            util.convert_data_for_cpp(data_directory)

if __name__ == "__main__":
    main(sys.argv[1:])