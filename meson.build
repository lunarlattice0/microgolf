project('microgolf', ['cpp', 'c'],
          version: '0.0.1',
    meson_version: '>= 0.62.0',
    default_options: [ 'warning_level=2', 'werror=false', 'cpp_std=gnu++2a', 'optimization=3',],
)
cmake = import('cmake')

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
    'src/rlImGui.cpp',
    'src/main.cpp',
#    'src/gui/imgui_lua.cpp',
    'src/stinky/stinky.cpp',
    'src/luahelper/luahelper.cpp',
]

microgolf_include_directories = [
    'src/vendor/imgui',
    'src/',
    'src/stinky',
    'src/gui',
    'src/luahelper',
]

imgui_source_files = [
    'src/vendor/imgui/imgui.cpp',
    'src/vendor/imgui/imgui_demo.cpp',
    'src/vendor/imgui/imgui_draw.cpp',
    'src/vendor/imgui/imgui_tables.cpp',
    'src/vendor/imgui/imgui_widgets.cpp',
    'src/vendor/imgui/misc/cpp/imgui_stdlib.cpp',
]

executable('microgolf', [microgolf_source_files, imgui_source_files], dependencies: microgolf_deps, include_directories: microgolf_include_directories,)

# Samples / Tests below

sample_imgui_source_files = [
    'src/tests/sample_imgui.cpp',
]

e_sample_imgui = executable('sample_imgui', ['src/rlImGui.cpp', sample_imgui_source_files, imgui_source_files], dependencies: microgolf_deps, include_directories: microgolf_include_directories,)
test('sample_imgui', e_sample_imgui)

sample_stinky_server_source_files = [
    'src/tests/sample_stinky_server.cpp',
    'src/stinky/stinky.cpp',
]
sample_stinky_server = executable('sample_stinky_server', ['src/rlImGui.cpp', sample_stinky_server_source_files, imgui_source_files], dependencies: microgolf_deps, include_directories: microgolf_include_directories,)
test('sample_stinky_server', sample_stinky_server)

sample_stinky_client_source_files = [
    'src/tests/sample_stinky_client.cpp',
    'src/stinky/stinky.cpp',
]

sample_stinky_client = executable('sample_stinky_client', ['src/rlImGui.cpp', sample_stinky_client_source_files, imgui_source_files], dependencies: microgolf_deps, include_directories: microgolf_include_directories,)
test('sample_stinky_client', sample_stinky_client)


#sample_lua_source_files = [
#    'src/tests/sample_lua.cpp',
#    'src/gui/imgui_lua.cpp',
#    'src/luahelper/luahelper.cpp',
#]
#sample_lua = executable('sample_lua', ['src/rlImGui.cpp', sample_lua_source_files, imgui_source_files], dependencies: microgolf_deps, include_directories: microgolf_include_directories,)
#test('sample_lua', sample_lua)
