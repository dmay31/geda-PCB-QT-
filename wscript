#!/usr/bin/env python
# encoding: ISO8859-1
# Thomas Nagy, 2005-2011

import os
import re


hidlist = ['qt']

files = [ 'src/action.c',
          'src/autoplace.c',
          'src/autoroute.c',
          'src/buffer.c',
          'src/change.c',
          'src/clip.c',
          'src/command.c',
          'src/compat.c',
          'src/copy.c',
          'src/create.c',
          'src/crosshair.c',
          'src/data.c',
          'src/dbus.c',
          'src/dbus-pcbmain.c',
          'src/djopt.c',
          'src/draw.c',
          'src/drill.c',
          'src/edif.y',
          'src/error.c',
          'src/file.c',
          'src/find.c',
          'src/flags.c',
          'src/fontmode.c',
          'src/free_atexit.c',
          'src/heap.c',
          'src/insert.c',
          'src/intersect.c',
          'src/line.c',
          'src/lrealpath.c',
          'src/main.c',
          'src/mirror.c',
          'src/misc.c',
          'src/move.c',
          'src/mtspace.c',
          'src/mymem.c',
          'src/netlist.c',
          'src/parse_l.l',
          'src/parse_y.y',
          'src/pcb-printf.c',
          'src/polygon.c',
          'src/polygon1.c',
          'src/puller.c',
          'src/print.c',
          'src/rats.c',
          'src/remove.c',
          'src/report.c',
          'src/res_lex.l',
          'src/res_parse.y',
          'src/rotate.c',
          'src/rtree.c',
          'src/rubberband.c',
          'src/search.c',
          'src/select.c',
          'src/set.c',
          'src/strflags.c',
          'src/thermal.c',
          'src/toporouter.c',
          'src/undo.c',
          'src/vector.c',
          'src/vendor.c',
          'src/hid/common/actions.c',
          'src/hid/common/flags.c',
          'src/hid/common/hidinit.c',
          'src/hid/common/hidnogui.c',
          'src/hid/common/extents.c',
          'src/hid/common/draw_helpers.c',
          'src/hid/common/hid_resource.c'
          ]

def options(opt):
    opt.load('compiler_cxx')
    opt.load('compiler_c')
    opt.load('qt4')
    opt.load('bison')
    opt.load('flex')

def configure(conf):
    conf.sub_config('src/hid/qt')
    conf.sub_config('gts')
    conf.load('gcc')
    conf.load('qt4')
    conf.load('bison')
    conf.load('flex')
    conf.load('gnu_dirs')
    conf.find_program('ld')
    conf.find_program('m4', var = 'GNUM4')
    conf.check_cfg(package='glib-2.0', args='--libs --cflags')
    conf.check_cfg(package='dbus-1', args='--libs --cflags')
    conf.check_cc( header_name="regex.h", mandatory=False)
    conf.check_cc( header_name="string.h", mandatory=False)
    conf.check_cc(function_name='regcomp', header_name="regex.h", mandatory=False)
    conf.check_cc(header_name='pwd.h')
    conf.check_cc(function_name='getpwuid', header_name='pwd.h')
    conf.check_cc(header_name='locale.h')
    conf.check_cc(header_name='unistd.h')


    conf.define( 'HAVE_DLFCN_H', 1, quote=False )
    conf.define( 'COORD_TYPE', 'long', quote=False )
    conf.define( 'COORD_MAX', 'LONG_MAX', quote=False )
    conf.define( 'PCB_DIR_SEPARATOR_S', os.sep)
    conf.define( 'PCB_DIR_SEPARATOR_C', os.sep )
    conf.define( 'PCB_PATH_DELIMETER', os.pathsep )
    conf.define( 'VERSION', "1.99z" )
    conf.define( 'HAVE_MKDIR', 1 )
    conf.define( '__C99__FUNC__', 1 )
    conf.define( '_GNU_SOURCE ', 1 )
    conf.define( 'MAKEDIR_TAKES_ONE_ARG', 1, quote=False )
    conf.define( 'GNUM4', conf.env.GNUM4 )
    conf.define( 'PREFIX', conf.env.PREFIX )
    conf.define( 'LIBRARYFILENAME', 'pcblib')
    conf.define( 'GETTEXT_PACKAGE', 'pcb')

    n1 = conf.root.find_node(conf.env.BINDIR)
    n2 = conf.root.find_node(conf.env.EXEC_PREFIX)
    conf.define( 'BINDIR_TO_EXECPREFIX', n2.path_from(n1) )

    n1 = conf.root.find_node(conf.env.DATADIR)
    n2 = conf.root.make_node('%s/pcb' % conf.env.DATADIR)
    conf.define( 'BINDIR_TO_PCBLIBDIR', n2.path_from(n1) )

    n2 = conf.root.make_node('%s/pcb/newlib' % conf.env.DATADIR)
    conf.define ( 'BINDIR_TO_PCBTREEDIR', n2.path_from(n1) )


    conf.write_config_header('config.h')

    hid_list = conf.root.make_node('hidlist.h')

    #Search for registration macros
    print 'Registrations'
    r = re.compile( "^REGISTER.*")
    out_file = open('src/core_lists.h', 'w')
    for f in files:
        fh = open(f, 'r')
        lines = fh.readlines()
        for line in lines:
            result = r.match(line)
            if result != None:
                out_file.write(result.group(0) + '\n')
        fh.close()
    out_file.close()

    #write out hidinit.h file
    print("Generating hidlist.h")
    fh = open('src/hid/common/hidlist.h', 'w')
    for hid in hidlist:
        fh.write('HID_DEF(%s)' % hid )
    fh.close()


    print("Building Resource files")
    res_in_file = open('src/pcb-menu.res.in', 'rU')
    res_file = open('src/pcb-menu.h', 'w')

    res_file.write('/* AUTOMATICALLY GENERATED FROM pcb-menu.res DO NOT EDIT */\n')
    lines = res_in_file.readlines()
    res_file.write( 'const char *pcb_menu_default[] = {\n')
    for line in lines:
        line = re.sub( "\"", '\\"', line )
        line = re.sub( '\n', "\",\n", line )
        line = re.sub( '^', '"', line )
        res_file.write( line )
    res_file.write('0};')
    res_file.close()
    res_in_file.close()


    res_in_file = open('src/gpcb-menu.res.in', 'rU')
    res_file = open('src/gpcb-menu.h', 'w')
    res_file.write('/* AUTOMATICALLY GENERATED FROM gpcb-menu.res DO NOT EDIT */\n')
    lines = res_in_file.readlines()
    res_file.write( 'const char *gpcb_menu_default[] = {\n')
    for line in lines:
        line = re.sub( "\"", '\\"', line )
        line = re.sub( '\n', "\",\n", line )
        line = re.sub( '^', '"', line )
        res_file.write( line )
    res_file.write('0};')
    res_file.close()
    res_in_file.close()


def build(bld):
     bld.add_subdirs('src/hid/qt')
     bld.add_subdirs('gts')

     b = bld.new_task_gen()
     b.env.CFLAGS += ['-std=c99']
     b.env.CFLAGS += ['-g']
     #b.env.LINKFLAGS += ['-M -Map=pcb_map.txt']
     b.target='pcb'
     for f in files:
         b.source += f + ' '
     b.includes = '. ./src ./gts'
     b.uselib = 'GLIB-2.0 LIBQT LIBGTS DBUS-1'
     b.features='c cxxprogram'
     b.lib = 'c m dl GL'
     b.use = 'GTSLIB LIBQT'
     b.defines = 'HAVE_CONFIG_H PCBLIBDIR="pcb" BINDIR="/usr/local/bin" PCBTREEPATH="pcb" LOCALEDIR="pcb" HOST="ME" HAVE_CONFIG_H=1  _POSIX_C_SOURCE=200112L'
     print("I'll build it")
