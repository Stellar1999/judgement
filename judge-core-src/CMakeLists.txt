cmake_minimum_required(VERSION 3.10)
project(judge-core C)

aux_source_directory(src _SUB_SOURCES)
aux_source_directory(src/json _JSON_LIB_SOURCES)


add_executable(judge-core ${_SUB_SOURCES} ${_JSON_LIB_SOURCES})
