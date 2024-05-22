from setuptools import Extension, setup

module = Extension("symnmf",
                  sources=[
                    'symnmf.c',
                    'symnmfmodule.c'
                  ])
setup(name='symnmf',
     version='1.0',
     description='C extension for symnmf',
     ext_modules=[module])