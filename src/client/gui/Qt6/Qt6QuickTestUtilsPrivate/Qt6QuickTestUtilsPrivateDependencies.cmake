# Make sure Qt6 is found before anything else.
set(Qt6QuickTestUtilsPrivate_FOUND FALSE)

if("${_qt_cmake_dir}" STREQUAL "")
    set(_qt_cmake_dir "${QT_TOOLCHAIN_RELOCATABLE_CMAKE_DIR}")
endif()
set(__qt_use_no_default_path_for_qt_packages "NO_DEFAULT_PATH")
if(QT_DISABLE_NO_DEFAULT_PATH_IN_QT_PACKAGES)
    set(__qt_use_no_default_path_for_qt_packages "")
endif()
find_dependency(Qt6 6.3.1
    PATHS
        "${CMAKE_CURRENT_LIST_DIR}/.."
        "${_qt_cmake_dir}"
        ${_qt_additional_packages_prefix_paths}
        ${QT_EXAMPLES_CMAKE_PREFIX_PATH}
    ${__qt_use_no_default_path_for_qt_packages}
)

# note: _third_party_deps example: "ICU\\;FALSE\\;1.0\\;i18n uc data;ZLIB\\;FALSE\\;\\;"
set(__qt_QuickTestUtilsPrivate_third_party_deps "")

foreach(__qt_QuickTestUtilsPrivate_target_dep ${__qt_QuickTestUtilsPrivate_third_party_deps})
    list(GET __qt_QuickTestUtilsPrivate_target_dep 0 __qt_QuickTestUtilsPrivate_pkg)
    list(GET __qt_QuickTestUtilsPrivate_target_dep 1 __qt_QuickTestUtilsPrivate_is_optional)
    list(GET __qt_QuickTestUtilsPrivate_target_dep 2 __qt_QuickTestUtilsPrivate_version)
    list(GET __qt_QuickTestUtilsPrivate_target_dep 3 __qt_QuickTestUtilsPrivate_components)
    list(GET __qt_QuickTestUtilsPrivate_target_dep 4 __qt_QuickTestUtilsPrivate_optional_components)
    set(__qt_QuickTestUtilsPrivate_find_package_args "${__qt_QuickTestUtilsPrivate_pkg}")
    if(__qt_QuickTestUtilsPrivate_version)
        list(APPEND __qt_QuickTestUtilsPrivate_find_package_args "${__qt_QuickTestUtilsPrivate_version}")
    endif()
    if(__qt_QuickTestUtilsPrivate_components)
        string(REPLACE " " ";" __qt_QuickTestUtilsPrivate_components "${__qt_QuickTestUtilsPrivate_components}")
        list(APPEND __qt_QuickTestUtilsPrivate_find_package_args COMPONENTS ${__qt_QuickTestUtilsPrivate_components})
    endif()
    if(__qt_QuickTestUtilsPrivate_optional_components)
        string(REPLACE " " ";" __qt_QuickTestUtilsPrivate_optional_components "${__qt_QuickTestUtilsPrivate_optional_components}")
        list(APPEND __qt_QuickTestUtilsPrivate_find_package_args OPTIONAL_COMPONENTS ${__qt_QuickTestUtilsPrivate_optional_components})
    endif()

    if(__qt_QuickTestUtilsPrivate_is_optional)
        if(${CMAKE_FIND_PACKAGE_NAME}_FIND_QUIETLY)
            list(APPEND __qt_QuickTestUtilsPrivate_find_package_args QUIET)
        endif()
        find_package(${__qt_QuickTestUtilsPrivate_find_package_args})
    else()
        find_dependency(${__qt_QuickTestUtilsPrivate_find_package_args})
    endif()
endforeach()

# Find Qt tool package.
set(__qt_QuickTestUtilsPrivate_tool_deps "")

if(__qt_QuickTestUtilsPrivate_tool_deps AND NOT "${QT_HOST_PATH}" STREQUAL "")
     # Make sure that the tools find the host tools first
     set(BACKUP_QuickTestUtilsPrivate_CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH})
     set(BACKUP_QuickTestUtilsPrivate_CMAKE_FIND_ROOT_PATH ${CMAKE_FIND_ROOT_PATH})
     list(PREPEND CMAKE_PREFIX_PATH "${QT_HOST_PATH_CMAKE_DIR}"
         "${_qt_additional_host_packages_prefix_paths}")
     list(PREPEND CMAKE_FIND_ROOT_PATH "${QT_HOST_PATH}"
         "${_qt_additional_host_packages_root_paths}")
endif()

foreach(__qt_QuickTestUtilsPrivate_target_dep ${__qt_QuickTestUtilsPrivate_tool_deps})
    list(GET __qt_QuickTestUtilsPrivate_target_dep 0 __qt_QuickTestUtilsPrivate_pkg)
    list(GET __qt_QuickTestUtilsPrivate_target_dep 1 __qt_QuickTestUtilsPrivate_version)

    unset(__qt_QuickTestUtilsPrivate_find_package_args)
    if(${CMAKE_FIND_PACKAGE_NAME}_FIND_QUIETLY)
        list(APPEND __qt_QuickTestUtilsPrivate_find_package_args QUIET)
    endif()
    if(${CMAKE_FIND_PACKAGE_NAME}_FIND_REQUIRED)
        list(APPEND __qt_QuickTestUtilsPrivate_find_package_args REQUIRED)
    endif()
    find_package(${__qt_QuickTestUtilsPrivate_pkg} ${__qt_QuickTestUtilsPrivate_version} ${__qt_QuickTestUtilsPrivate_find_package_args}
        PATHS
            ${_qt_additional_packages_prefix_paths}
    )
    if (NOT ${__qt_QuickTestUtilsPrivate_pkg}_FOUND)
        if(NOT "${QT_HOST_PATH}" STREQUAL "")
             set(CMAKE_PREFIX_PATH ${BACKUP_QuickTestUtilsPrivate_CMAKE_PREFIX_PATH})
             set(CMAKE_FIND_ROOT_PATH ${BACKUP_QuickTestUtilsPrivate_CMAKE_FIND_ROOT_PATH})
        endif()
        return()
    endif()
endforeach()
if(__qt_QuickTestUtilsPrivate_tool_deps AND NOT "${QT_HOST_PATH}" STREQUAL "")
     set(CMAKE_PREFIX_PATH ${BACKUP_QuickTestUtilsPrivate_CMAKE_PREFIX_PATH})
     set(CMAKE_FIND_ROOT_PATH ${BACKUP_QuickTestUtilsPrivate_CMAKE_FIND_ROOT_PATH})
endif()

# note: target_deps example: "Qt6Core\;5.12.0;Qt6Gui\;5.12.0"
set(__qt_QuickTestUtilsPrivate_target_deps "Qt6Core\;6.3.1;Qt6Test\;6.3.1;Qt6Qml\;6.3.1;Qt6QuickPrivate\;6.3.1;Qt6Quick\;6.3.1;Qt6QuickTest\;6.3.1")
set(__qt_QuickTestUtilsPrivate_find_dependency_paths "${CMAKE_CURRENT_LIST_DIR}/.." "${_qt_cmake_dir}")
_qt_internal_find_dependencies(__qt_QuickTestUtilsPrivate_target_deps __qt_QuickTestUtilsPrivate_find_dependency_paths)

set(_Qt6QuickTestUtilsPrivate_MODULE_DEPENDENCIES "Core;Test;Qml;QmlPrivate;QuickPrivate;Quick;QuickTest")
set(Qt6QuickTestUtilsPrivate_FOUND TRUE)
