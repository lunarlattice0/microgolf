project('microgolf', ['cpp', 'c'],
          version: '0.0.1',
    meson_version: '>= 0.62.0',
    default_options: [ 'warning_level=2', 'werror=false', 'cpp_std=gnu++2a', 'optimization=3',],
)

raylibdep = dependency('raylib')

microgolf_deps = [
    raylibdep,
]

microgolf_source_files = [
    'src/sample_main.cpp',
    'src/rlImGui.cpp',
]

microgolf_include_directories = [
    'src/vendor/imgui',
    'src/',
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
