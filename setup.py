from setuptools import Extension, setup

module = Extension("symnmfmodule",
                  sources=[
                    'symnmf.c',
                    'symnmfmodule.c'
                  ])
setup(name='symnmfmodule',
     version='1.0',
     description='C extension for symnmf',
     ext_modules=[module])