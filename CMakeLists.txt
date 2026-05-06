cmake_minimum_required(VERSION 3.16)

# ESP-IDF Build-System einbinden (muss vor project() stehen)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# Hauptkomponente liegt in src/ statt main/
set(EXTRA_COMPONENT_DIRS "src")

project(cyd-display-car1)
