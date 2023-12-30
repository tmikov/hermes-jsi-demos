# This is free and unencumbered software released into the public domain.
# For more information, please refer to the LICENSE file in the root directory
# or at <http://unlicense.org/>

cmake_minimum_required(VERSION 3.22)
project(hermes-jsi-demos)

add_subdirectory(hello)
add_subdirectory(runner)
add_subdirectory(host-functions)
add_subdirectory(hf-runner)

# A target to build all demos
add_custom_target(demos
        DEPENDS hello runner host-functions hf-runner)
