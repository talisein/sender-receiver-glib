

curl_dep = dependency('libcurl')

gnome = import('gnome')
res = gnome.compile_resources('examples-resources',
                              'examples.gresource.xml',
                              install_header : false,
                              export : false,
                              c_name : 'example_resources')

hello_world_exe = executable('hello-world', 'http_downloader.cpp', res,
                             dependencies: [gtkmm_dep, gtkmm_stdexec_dep, curl_dep])
