#!/usr/bin/python3
import getopt
import os

import sys
import yaml

def main(argv):
    try:
        opts, args = getopt.getopt(argv,"hc:")
    except getopt.GetoptError:
        print('genRunConfig.py -c <runner_configuration>')
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print('genRunConfig.py -c <runner_configuration>')
            sys.exit()
        elif opt in ("-c"):
            runner_configuration = arg

    with open(runner_configuration) as file_handle:
        config_data = yaml.load(file_handle, Loader=yaml.FullLoader)

    out_dir = config_data["outdir"]
    base_config = config_data["base"]
    data_sets = config_data["datasets"]
    com_intervals = config_data["comintervals"]
    schemes = config_data["schemes"]

    i = 0

    for i_ds, data_set in enumerate(data_sets):
        com_interval_set = com_intervals[i_ds]
        for i_ci, com_interval in enumerate(com_interval_set):
            schema_set = schemes[i_ds]
            for schema in schema_set:

                configuration = {'configuration' : {'base': base_config, 'dataSubPath': "/data/data_" + data_set + "/" ,'mpiParameters': { 'schema': schema, 'comInterval' : com_interval}}}
                path = os.path.join(out_dir + data_set + '/')
                filename = str(i) + '_' + schema + '_' + str(com_interval) + '.yaml'
                try:
                    os.makedirs(path)
                except OSError as error:
                    pass

                with open(path + filename, 'w') as f:
                    yaml.dump(configuration, f, sort_keys=False, default_flow_style=False)

                i += 1

    return

if __name__ == "__main__":
    main(sys.argv[1:])
