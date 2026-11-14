# Build step of the submodule projects from submoduleTargets.cmake, run as a script:
#
#   cmake -DNAME=<name> -DBINARY_DIR=<dir> [-DCONFIG=<config>] [-DGENERATOR=<generator>] [-DBOOTSTRAP=TRUE] -P buildSubmodule.cmake
#
# BOOTSTRAP is meant for projects that locate their own dependencies while configuring, after having built them.
# Rasteron does so for zlib, libpng, libtiff and freetype, which takes a build, a second configure and another build.
# The first build is allowed to fail and continues past errors, its output is kept inside of a log file

if(NOT BINARY_DIR OR NOT EXISTS "${BINARY_DIR}/CMakeCache.txt")
    message(FATAL_ERROR "${NAME}: no configured build inside of ${BINARY_DIR}")
endif()

set(build_command ${CMAKE_COMMAND} --build ${BINARY_DIR})
if(CONFIG)
    list(APPEND build_command --config ${CONFIG})
endif()
if(NOT CMAKE_VERSION VERSION_LESS 3.12)
    include(ProcessorCount)
    ProcessorCount(job_count)
    if(job_count GREATER 1)
        list(APPEND build_command --parallel ${job_count})
    endif()
endif()

set(stamp "${BINARY_DIR}/bootstrap.stamp")
set(log "${BINARY_DIR}/bootstrap.log")

if(BOOTSTRAP AND NOT EXISTS ${stamp})
    set(keep_going "") # builds as much as possible, so that every dependency gets installed
    if(GENERATOR MATCHES "Ninja")
        set(keep_going -- -k 0)
    elseif(GENERATOR MATCHES "Makefiles")
        set(keep_going -- -k)
    endif()

    message(STATUS "${NAME}: building dependencies on a first pass, this can take several minutes. Output is written to ${log}")
    execute_process(COMMAND ${build_command} ${keep_going} OUTPUT_FILE ${log} ERROR_FILE ${log})

    message(STATUS "${NAME}: configuring again to locate the dependencies")
    execute_process(COMMAND ${CMAKE_COMMAND} ${BINARY_DIR} RESULT_VARIABLE result)
    if(result)
        message(FATAL_ERROR "${NAME}: configuring failed, see ${log} for the first pass")
    endif()
endif()

execute_process(COMMAND ${build_command} RESULT_VARIABLE result)
if(result AND BOOTSTRAP AND EXISTS ${stamp}) # dependencies of the project may have changed since the first pass
    message(STATUS "${NAME}: build failed, configuring again before a second attempt")
    execute_process(COMMAND ${CMAKE_COMMAND} ${BINARY_DIR})
    execute_process(COMMAND ${build_command} RESULT_VARIABLE result)
endif()
if(result)
    message(FATAL_ERROR "${NAME}: build failed")
endif()

if(BOOTSTRAP)
    file(WRITE ${stamp} "Dependencies of ${NAME} were built and located\n")
endif()
