project('microgolf', ['cpp', 'c'],
          version: '0.0.1',
    meson_version: '>= 0.62.0',
    default_options: [ 'warning_level=2', 'werror=false', 'cpp_std=gnu++2a', 'optimization=3',],
)
cmake = import('cmake')

prod_assets_dir = 'assets/out'

opt_var = cmake.subproject_options()
opt_var.add_cmake_defines({'CMAKE_BUILD_TYPE': 'RelWithDebInfo'})
luau_subproj = cmake.subproject('luau', options: opt_var)

luau_compiler_dep = luau_subproj.dependency('Luau.Compiler')
luau_vm_dep = luau_subproj.dependency('Luau.VM')
luau_ast_dep = luau_subproj.dependency('Luau.Ast')

raylibdep = dependency('raylib')
enetdep = dependency('libenet')
libsodiumdep = dependency('libsodium')

microgolf_deps = [
    raylibdep,
    enetdep,
    libsodiumdep,
    luau_compiler_dep,
    luau_vm_dep,
    luau_ast_dep,
]

microgolf_source_files = [
    'src/main.cpp',

    # imgui cpps
    'src/vendor/rlImGui/rlImGui.cpp',
    'src/vendor/imgui/imgui.cpp',
    'src/vendor/imgui/imgui_draw.cpp',
    'src/vendor/imgui/imgui_tables.cpp',
    'src/vendor/imgui/imgui_widgets.cpp',
    'src/vendor/imgui/misc/cpp/imgui_stdlib.cpp',

    'src/stinky/stinky.cpp',
    'src/putrid/putrid.cpp',
    'src/putrid/mapeditor/mapeditor.cpp',

    'src/putrid/luahelper/luahelper.cpp',
]

microgolf_include_directories = [
    'src/vendor/rlImGui',
    'src/vendor/imgui',
    'src/vendor/raygui',
    'src/',
    'src/stinky',
    'src/putrid',
]

microgolf = executable('microgolf',
[microgolf_source_files],
dependencies: microgolf_deps,
include_directories: microgolf_include_directories,
)
# TODO: make a production export target
