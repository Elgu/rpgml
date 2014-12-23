#!/bin/bash

if [ $# -lt 1 ]; then
  echo "Usage: new_Node_plugin.sh <PluginName>"
  exit 1
fi 

PluginName=$1

# TODO: Use a better path than ".."
cat "../templates/RPGML_Node_TEMPLATE.h" | sed "s/TEMPLATE/$PluginName/g" > "RPGML_Node_$PluginName.h"
cat "../templates/RPGML_Node_TEMPLATE.cpp" | sed "s/TEMPLATE/$PluginName/g" > "RPGML_Node_$PluginName.cpp"

