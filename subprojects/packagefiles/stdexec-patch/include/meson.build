conf_data = configuration_data()
conf_data.set('STDEXEC_VERSION_MAJOR', meson.project_version().split('.')[0])
conf_data.set('STDEXEC_VERSION_MINOR', meson.project_version().split('.')[1])
conf_data.set('STDEXEC_VERSION_PATCH', '0')
conf_data.set_quoted('STDEXEC_VERSION', meson.project_version())

configure_file(output : 'stdexec_version_config.hpp',
               configuration : conf_data)
