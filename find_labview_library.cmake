# We check if the uses hasn't already specified the path
if(NOT DEFINED cintools_PATH)
    if(WIN32)
        # Add to CMAKE_IGNORE_PREFIX_PATH based on bitness
        if(CMAKE_SIZEOF_VOID_P EQUAL 4)
            # 32-bit ignore usual 64-bit path
            list(APPEND CMAKE_IGNORE_PREFIX_PATH "C:/Program Files")
        else()
            # 64-bit ignore usual 32-bit path
            list(APPEND CMAKE_IGNORE_PREFIX_PATH "C:/Program Files (x86)")
        endif()

        SET(LABVIEW_VERSIONS_LIST_PREPEND "National Instruments/LabVIEW ")
        SET(LABVIEW_VERSIONS_LIST_APPEND "/cintools")
        SET(LABVIEW_LIBRARY_NAMES "LabVIEWv" "LabVIEW")
    elseif(UNIX)
        SET(LABVIEW_VERSIONS_LIST_PREPEND "../natinst/LabVIEW-")

        if(CMAKE_SIZEOF_VOID_P EQUAL 4)
            # 32-bit
            SET(LABVIEW_VERSIONS_LIST_APPEND "-32/cintools")
        else()
            # 64-bit
            SET(LABVIEW_VERSIONS_LIST_APPEND "-64/cintools")
        endif()

        SET(LABVIEW_LIBRARY_NAMES "liblv.so")
    endif()

    # Just the version numbers to build into a list of search loctions
    set(supported_lv_versions_LIST
        2024
        2023
        2022
        2021
        2020
        2019
        2018
        2017
        2016
        2015
        2014
        2013
    )

    # Transform list with Prepend and Append to get National Instruments/LabVIEW 20xx/cintools
    list(TRANSFORM supported_lv_versions_LIST PREPEND ${LABVIEW_VERSIONS_LIST_PREPEND})
    list(TRANSFORM supported_lv_versions_LIST APPEND ${LABVIEW_VERSIONS_LIST_APPEND})

    find_path(cintools_PATH
        extcode.h
        PATH_SUFFIXES ${supported_lv_versions_LIST}
    )

    if(cintools_PATH)
        message(STATUS "Looking for LabVIEW library")
    else()
        message(FATAL_ERROR "Failed to find the location of LabVIEW's extcode.h file from cintools. Specify the value manually using  the cintools_PATH variable")
    endif()
endif()

find_library(LABVIEW_LIB
    NAMES ${LABVIEW_LIBRARY_NAMES}
    HINTS ${cintools_PATH}
)
message(STATUS "Suitable LabVIEW Library found at ${LABVIEW_LIB}")

add_library(labview INTERFACE)
target_link_libraries(labview INTERFACE ${LABVIEW_LIB})
target_include_directories(labview INTERFACE ${cintools_PATH})