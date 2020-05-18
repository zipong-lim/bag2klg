set(REALSENSE_ROOT "/usr/local" CACHE PATH "Root directory of libREALSENSE")

find_path(REALSENSE_INCLUDE_DIR NAMES librealsense librealsense2 HINTS "${REALSENSE_ROOT}/include")
find_library(REALSENSE_LIBRARY NAMES realsense realsense2 HINTS "${REALSENSE_ROOT}/bin/x64" "${REALSENSE_ROOT}/lib")

find_package_handle_standard_args(REALSENSE DEFAULT_MSG REALSENSE_LIBRARY REALSENSE_INCLUDE_DIR)

mark_as_advanced(REALSENSE_LIBRARY REALSENSE_INCLUDE_DIR)

