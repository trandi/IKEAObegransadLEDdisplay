set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(srcs)

set(requires "bluepad32")

# See: https://gitlab.com/ricardoquesada/bluepad32/-/issues/9
if(DEFINED ENV{BLUEPAD32_ARDUINO})
    list(APPEND srcs)
    list(APPEND requires "bluepad32_arduino" "arduino")
endif()

idf_component_register(SRCS "main.c" "main.cpp" "Display.cpp" "${srcs}"
                    INCLUDE_DIRS "."
                    REQUIRES "${requires}")


