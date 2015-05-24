#!/bin/bash

if [ $# -lt 1 ]; then
  echo "Usage: new_Node_plugin.sh <PluginName>"
  exit 1
fi 

PluginName=$1
PluginHeader="RPGML_Node_${PluginName}.h"
PluginCPP="RPGML_Node_${PluginName}.cpp"

if [ -e "$PluginHeader" -o -e "$PluginCPP" ]; then
  echo "Plugin '$PluginName' already exists"
  exit 4
fi

function get_namespaces()
{
  destdir=$(pwd)

  namespaces=""
  currdir=$(basename "$(pwd)")
  while [ -n "$currdir" -a "$currdir" != "ROOT" ]; do
    # reverse order
    namespaces="${namespaces} ${currdir}"
    cd ..
    currdir=$(basename "$(pwd)")
  done

  if [ "$currdir" != "ROOT" ]; then
    echo "Script must be called within RPGML's ROOT directory"
    exit 3
  fi

  ROOT_dir="$(pwd)"
  cd "$destdir"
}

function filter() # in, out
{
  if [ "$#" != "2" ]; then
    echo "function filter() requires 2 arguments"
    exit 2
  fi

  in="$1"
  out="$2"

  cat "$in" \
     | sed \
       -e "s/Node_TEMPLATE_h/Node_""$INCLUDE_GUARD_NAMESPACES""$PluginName""_h/g" \
       -e "s/TEMPLATE_NAME/$PLUGIN_NAMESPACES_NAME$PluginName/g" \
       -e "s/TEMPLATE/$PluginName/g" \
       -e "s/EXTRA_NAMESPACES/$EXTRA_NAMESPACES/g" \
       -e "s/ADDITIONAL_NAMESPACES_BEGIN/$ADDITIONAL_NAMESPACES_BEGIN/g" \
       -e "s/ADDITIONAL_NAMESPACES_END/$ADDITIONAL_NAMESPACES_END \/\/ $ADDITIONAL_NAMESPACES_BEGIN/g" \
    > "$out"
}

get_namespaces

INCLUDE_GUARD_NAMESPACES=""
EXTRA_NAMESPACES=""
ADDITIONAL_NAMESPACES_BEGIN=""
ADDITIONAL_NAMESPACES_END=""
PLUGIN_NAMESPACES_NAME=""
for n in $namespaces; do
  EXTRA_NAMESPACES="$n::$EXTRA_NAMESPACES"
  INCLUDE_GUARD_NAMESPACES="$n""_""$INCLUDE_GUARD_NAMESPACES"
  ADDITIONAL_NAMESPACES_BEGIN="namespace ${n} { $ADDITIONAL_NAMESPACES_BEGIN"
  ADDITIONAL_NAMESPACES_END="$ADDITIONAL_NAMESPACES_END }"
  PLUGIN_NAMESPACES_NAME="$n"".""$PLUGIN_NAMESPACES_NAME"
done

#echo EXTRA_NAMESPACES=${EXTRA_NAMESPACES}
#echo ADDITIONAL_NAMESPACES_BEGIN="'"${ADDITIONAL_NAMESPACES_BEGIN}"'"
#echo ADDITIONAL_NAMESPACES_END="'"${ADDITIONAL_NAMESPACES_END}"'"
#echo ROOT_dir="'"${ROOT_dir}"'"
#echo INCLUDE_GUARD_NAMESPACES="'"${INCLUDE_GUARD_NAMESPACES}"'"
#echo PLUGIN_NAMESPACES_NAME="'"${PLUGIN_NAMESPACES_NAME}"'"

# Templates are relative to the script
filter "$(dirname $0)""/../templates/RPGML_Node_TEMPLATE.h" "$PluginHeader"
filter "$(dirname $0)""/../templates/RPGML_Node_TEMPLATE.cpp" "$PluginCPP"

echo "Created $PluginHeader $PluginCPP"

