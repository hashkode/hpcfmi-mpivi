#!/bin/bash
cd "$(dirname "$0")"

### init routine
echo "> generating PlantUML diagrams"

path=$(pwd)"/puml/"
gen_dir="../gen/puml/"
rm -rf ${gen_dir}

java -jar /usr/bin/plantuml.jar ${path} -o ${gen_dir} -tsvg
