project('sender-receiver-glib', ['cpp'],
        license : 'LGPL-3.0-or-later',
        version : '0.0.1',
        default_options : ['cpp_std=c++23'])

stdexec_dep = dependency('stdexec',
                         fallback: ['stdexec'])

glibmm_dep = dependency('glibmm-2.68')
gtkmm_dep = dependency('gtkmm-4.0')

subdir('src')

boostut_dep = dependency('ut',
                         method: 'cmake',
                         modules: 'Boost::ut',
                         fallback: ['boostut', 'boostut_dep'])

subdir('test')
subdir('examples')
