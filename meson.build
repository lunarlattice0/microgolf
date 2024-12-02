project('microgolf', ['cpp', 'c'],
          version: '0.0.1',
    meson_version: '>= 0.62.0',
    default_options: [ 'warning_level=2', 'werror=false', 'cpp_std=gnu++2a', 'optimization=3',],
)

raylibdep = dependency('raylib')
enetdep = dependency('libenet')
libsodiumdep = dependency('libsodium')

microgolf_deps = [
    raylibdep,
    enetdep,
    libsodiumdep,
]

microgolf_source_files = [
    'src/rlImGui.cpp',
    'src/main.cpp',
]

microgolf_include_directories = [
    'src/vendor/imgui',
    'src/',
    'src/stinky',
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

sample_stinky_basic_source_files = [
    'src/tests/sample_stinky_basic.cpp',
    'src/stinky/stinky.cpp',
]
e_sample_stinky = executable('sample_stinky_basic', ['src/rlImGui.cpp', sample_stinky_basic_source_files, imgui_source_files], dependencies: microgolf_deps, include_directories: microgolf_include_directories,)
test('sample_stinky_basic', e_sample_stinky)
