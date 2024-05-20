from setuptools import Extension, setup

module = Extension("symmnf",
                  sources=[
                    'symmnf.c',
                    'symmnfmodule.c'
                  ])
setup(name='symmnf',
     version='1.0',
     description='C extension for symmnf',
     ext_modules=[module])