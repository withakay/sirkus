#!/bin/zsh

# set pwd to the directory of the script
cd "$(dirname "$0")"

# check a plugin with type was provided
plugin_type="$1"
if [ -z "$plugin_type" ]; then
    echo "Usage: $0 <plugin>"
    exit 1
fi

# create the filtergraph filename (a filtergraph is just AudioPluginHost's way of saving a preset)
filtergraph="${plugin_type}.filtergraph"

# check if the filtergraph exists
if [ ! -f "$filtergraph" ]; then
    echo "File not found: $filtergraph"
    echo "To create a filtergraph open AudioPluginHost.app and drag and drop the plugin you want to load."
    echo "Save the filtergraph with the name $filtergraph in this folder"
    exit 1
fi

# open AudioPluginHost with the filtergraph
 AudioPluginHost.app/Contents/MacOS/AudioPluginHost "$filtergraph"