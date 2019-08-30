project(HShell)

add_subdirectory(AndroidDef)

aux_source_directory(. RootSrc)

add_library(HShell SHARED ${RootSrc})

find_library(
        log-lib
        log)

# dl 动态链接； z 压缩
target_link_libraries(HShell ${log-lib}  android dl z AndroidDef)