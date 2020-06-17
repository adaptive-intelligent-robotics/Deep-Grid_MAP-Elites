#! /usr/bin/env python
import sys
import os
import sferes
sys.path.insert(0, sys.path[0]+'/waf_tools')

from waflib.Configure import conf

import dart
import corrade
import magnum
import magnum_integration
import magnum_plugins
import robot_dart

def options(opt):
    opt.load('dart')
    opt.load('corrade')
    opt.load('magnum')
    opt.load('magnum_integration')
    opt.load('magnum_plugins')
    opt.load('robot_dart')

@conf
def configure(conf):
    print('conf exp:')
    conf.load('dart')
    conf.load('corrade')
    conf.load('magnum')
    conf.load('magnum_integration')
    conf.load('magnum_plugins')
    conf.load('robot_dart')


    conf.check_dart()
    conf.check_corrade(components='Utility PluginManager', required=False)
    conf.env['magnum_dep_libs'] = 'MeshTools Primitives Shaders SceneGraph GlfwApplication'
    if conf.env['DEST_OS'] == 'darwin':
        conf.env['magnum_dep_libs'] += ' WindowlessCglApplication'
    else:
        conf.env['magnum_dep_libs'] += ' WindowlessGlxApplication'
    conf.check_magnum(components=conf.env['magnum_dep_libs'], required=False)
    conf.check_magnum_plugins(components='AssimpImporter', required=False)
    conf.check_magnum_integration(components='Dart', required=False)

    if len(conf.env.INCLUDES_MagnumIntegration) > 0:
        conf.get_env()['BUILD_MAGNUM'] = True
        conf.env['magnum_libs'] = magnum.get_magnum_dependency_libs(conf, conf.env['magnum_dep_libs']) + magnum_integration.get_magnum_integration_dependency_libs(conf, 'Dart')
    conf.check_robot_dart()
    
def build(bld):

    # Quick and dirty link to python C++
    bld.env.LIBPATH_PYTHON = '/usr/lib/x86_64-linux-gnu/'
    bld.env.LIB_PYTHON = [ 'python3.6m']
    bld.env.INCLUDES_PYTHON = '/usr/include/python3.6m'

    sferes.create_variants(bld,
                           source = 'main.cpp',
                           includes = '. ../../',
                           uselib = 'TBB BOOST EIGEN PTHREAD MPI ROBOTDART DART',
                           use = 'sferes2',
                           target = 'experiment',
                           variants = ['RASTRIGIN', 'HEXA_OMNI', 'ARM_VAR'])
    
    bld.program(features = 'cxx',
                source = 'main_analysis.cpp',
                includes = '. ../../',
                uselib = 'BOOST EIGEN PYTHON',
                use = 'sferes2',
                target = 'analysis')
