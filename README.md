make -C ${IDF_PATH}/tools/unit-test-app EXTRA_COMPONENT_DIRS=`pwd`/components TEST_COMPONENTS="network" flash monitor
