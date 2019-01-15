project(HShell)
aux_source_directory(. RootSrc)

add_library(HShell SHARED ${RootSrc})

# dl 动态链接； z 压缩
target_link_libraries(HShell android dl z)