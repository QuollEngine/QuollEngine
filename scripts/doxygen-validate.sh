doxygen

CURRENT_DIRECTORY=$(pwd)

if [ -s "doxy.warn" ]; then
    cat doxy.warn | sed "s|${CURRENT_DIRECTORY}|.|"
    echo "Total: $(wc -l < doxy.warn) documentation warnings"
    exit 1;
else
    echo "Documentation is valid!"
    exit 0;
fi
