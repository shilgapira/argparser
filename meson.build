project('argparser', 'c',
  license: 'MIT',
  version: 'v0.1.1',
  default_options: [
    'prefix=/usr',
    'c_std=c2x',
    'bindir=bin',
    'b_sanitize=address',
    'b_lundef=false',
    'warning_level=2'
  ]
)

cc = meson.get_compiler('c')

cflags = cc.get_supported_arguments([
  '-Werror=declaration-after-statement',
  '-Werror=implicit-function-declaration',
  '-Wno-cast-function-type',
  '-Wno-missing-field-initializers',
  '-Wno-unused-parameter'
])

add_project_arguments(cflags, language: 'c')

library('argparser', 'argparser.c', install: true)
