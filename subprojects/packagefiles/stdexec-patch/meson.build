project('stdexec', ['cpp'],
        license : 'Apache-2.0',
        version : '25.09')

stdexec_inc = include_directories('include')
thread_dep = dependency('threads')
subdir('include')

stdexec_dep = declare_dependency(version: meson.project_version(),
                                 include_directories: stdexec_inc,
                                 dependencies: thread_dep)
meson.override_dependency('stdexec', stdexec_dep)


if get_option('build-examples')
  stdexec_examples = [
    ['example.hello_world', 'examples/hello_world.cpp'],
    ['example.hello_coro', 'examples/hello_coro.cpp'],
    ['example.scope', 'examples/scope.cpp'],
    ['example.retry', 'examples/retry.cpp'],
    ['example.then', 'examples/then.cpp'],
    ['example.server_theme.let_value', 'examples/server_theme/let_value.cpp'],
    ['example.server_theme.on_transfer', 'examples/server_theme/on_transfer.cpp'],
    ['example.server_theme.then_upon', 'examples/server_theme/then_upon.cpp'],
    ['example.server_theme.split_bulk', 'examples/server_theme/split_bulk.cpp'],
    ['example.benchmark.static_thread_pool', 'examples/benchmark/static_thread_pool.cpp'],
    ['example.benchmark.static_thread_pool_old', 'examples/benchmark/static_thread_pool_old.cpp'],
    ['example.benchmark.static_thread_pool_nested', 'examples/benchmark/static_thread_pool_nested.cpp'],
    ['example.benchmark.static_thread_pool_nested_old', 'examples/benchmark/static_thread_pool_nested_old.cpp'],
    ['example.benchmark.static_thread_pool_bulk_enqueue', 'examples/benchmark/static_thread_pool_bulk_enqueue.cpp'],
    ['example.benchmark.static_thread_pool_bulk_enqueue_nested', 'examples/benchmark/static_thread_pool_bulk_enqueue_nested.cpp'],
  ]

  cpp_compiler = meson.get_compiler('cpp')
  if cpp_compiler.has_header('linux/io_uring.h')
    stdexec_examples += [['example.io_uring', 'examples/io_uring.cpp']]
  endif

  foreach stdexec_example : stdexec_examples
    stdexec_example_exe = executable(stdexec_example[0], stdexec_example[1],
                                     dependencies: stdexec_dep)
  endforeach
endif

if get_option('build-tests')
  catch2_dep = dependency('catch2', version: '<3.0.0')

  stdexec_tests_exe = executable('tests', 'test/test_main.cpp',
                                 'test/stdexec/algos/adaptors/test_bulk.cpp',
                                 'test/stdexec/algos/adaptors/test_continues_on.cpp',
                                 'test/stdexec/algos/adaptors/test_ensure_started.cpp',
                                 'test/stdexec/algos/adaptors/test_into_variant.cpp',
                                 'test/stdexec/algos/adaptors/test_let_error.cpp',
#                                 'test/stdexec/algos/adaptors/test_let_fail1.cpp',
                                 'test/stdexec/algos/adaptors/test_let_stopped.cpp',
                                 'test/stdexec/algos/adaptors/test_let_value.cpp',
                                 'test/stdexec/algos/adaptors/test_on2.cpp',
                                 'test/stdexec/algos/adaptors/test_on3.cpp',
                                 'test/stdexec/algos/adaptors/test_on.cpp',
                                 'test/stdexec/algos/adaptors/test_schedule_from.cpp',
                                 'test/stdexec/algos/adaptors/test_split.cpp',
                                 'test/stdexec/algos/adaptors/test_starts_on.cpp',
                                 'test/stdexec/algos/adaptors/test_stopped_as_error.cpp',
                                 'test/stdexec/algos/adaptors/test_stopped_as_optional.cpp',
                                 'test/stdexec/algos/adaptors/test_then.cpp',
#                                 'test/stdexec/algos/adaptors/test_then_fail1.cpp',
                                 'test/stdexec/algos/adaptors/test_transfer_when_all.cpp',
                                 'test/stdexec/algos/adaptors/test_upon_error.cpp',
                                 'test/stdexec/algos/adaptors/test_upon_stopped.cpp',
                                 'test/stdexec/algos/adaptors/test_when_all.cpp',

                                 'test/stdexec/algos/consumers/test_start_detached.cpp',
                                 'test/stdexec/algos/consumers/test_sync_wait.cpp',

                                 'test/stdexec/algos/factories/test_just.cpp',
                                 'test/stdexec/algos/factories/test_just_error.cpp',
                                 'test/stdexec/algos/factories/test_just_stopped.cpp',
                                 'test/stdexec/algos/factories/test_read.cpp',
                                 'test/stdexec/algos/factories/test_transfer_just.cpp',

                                 'test/stdexec/algos/other/test_execute.cpp',

                                 'test/stdexec/concepts/test_awaitables.cpp',
                                 'test/stdexec/concepts/test_concept_operation_state.cpp',
                                 'test/stdexec/concepts/test_concept_scheduler.cpp',
                                 'test/stdexec/concepts/test_concepts_receiver.cpp',
                                 'test/stdexec/concepts/test_concepts_sender.cpp',

                                 'test/stdexec/cpos/test_cpo_bulk.cpp',
                                 'test/stdexec/cpos/test_cpo_connect.cpp',
                                 'test/stdexec/cpos/test_cpo_ensure_started.cpp',
                                 'test/stdexec/cpos/test_cpo_receiver.cpp',
                                 'test/stdexec/cpos/test_cpo_schedule.cpp',
                                 'test/stdexec/cpos/test_cpo_split.cpp',
                                 'test/stdexec/cpos/test_cpo_start.cpp',
                                 'test/stdexec/cpos/test_cpo_upon_error.cpp',
                                 'test/stdexec/cpos/test_cpo_upon_stopped.cpp',

                                 'test/stdexec/detail/test_completion_signatures.cpp',
                                 'test/stdexec/detail/test_utility.cpp',

                                 'test/stdexec/queries/test_forwarding_queries.cpp',
                                 'test/stdexec/queries/test_get_forward_progress_guarantee.cpp',
                                 include_directories: include_directories('test'),
                                 dependencies: [stdexec_dep, catch2_dep])
  test('stdexec_tests', stdexec_tests_exe)
endif
#doxygen = find_program('doxygen')

stdexec_public_headers = [
  meson.project_source_root() / 'include/exec/any_sender_of.hpp',
  meson.project_source_root() / 'include/exec/async_scope.hpp',
  meson.project_source_root() / 'include/exec/at_coroutine_exit.hpp',
  meson.project_source_root() / 'include/exec/create.hpp',
  meson.project_source_root() / 'include/exec/env.hpp',
  meson.project_source_root() / 'include/exec/finally.hpp',
  meson.project_source_root() / 'include/exec/inline_scheduler.hpp',
  meson.project_source_root() / 'include/exec/into_tuple.hpp',
  meson.project_source_root() / 'include/exec/just_from.hpp',
  meson.project_source_root() / 'include/exec/libdispatch_queue.hpp',
  meson.project_source_root() / 'include/exec/linux/io_uring_context.hpp',
  meson.project_source_root() / 'include/exec/linux/memory_mapped_region.hpp',
  meson.project_source_root() / 'include/exec/linux/safe_file_descriptor.hpp',
  meson.project_source_root() / 'include/exec/materialize.hpp',
  meson.project_source_root() / 'include/exec/on.hpp',
  meson.project_source_root() / 'include/exec/on_coro_disposition.hpp',
  meson.project_source_root() / 'include/exec/repeat_effect_until.hpp',
  meson.project_source_root() / 'include/exec/repeat_n.hpp',
  meson.project_source_root() / 'include/exec/reschedule.hpp',
  meson.project_source_root() / 'include/exec/scope.hpp',
  meson.project_source_root() / 'include/exec/sequence.hpp',
  meson.project_source_root() / 'include/exec/sequence/any_sequence_of.hpp',
  meson.project_source_root() / 'include/exec/sequence/empty_sequence.hpp',
  meson.project_source_root() / 'include/exec/sequence/ignore_all_values.hpp',
  meson.project_source_root() / 'include/exec/sequence/iterate.hpp',
  meson.project_source_root() / 'include/exec/sequence/transform_each.hpp',
  meson.project_source_root() / 'include/exec/sequence_senders.hpp',
  meson.project_source_root() / 'include/exec/single_thread_context.hpp',
  meson.project_source_root() / 'include/exec/start_now.hpp',
  meson.project_source_root() / 'include/exec/static_thread_pool.hpp',
  meson.project_source_root() / 'include/exec/system_context.hpp',
  meson.project_source_root() / 'include/exec/task.hpp',
  meson.project_source_root() / 'include/exec/timed_scheduler.hpp',
  meson.project_source_root() / 'include/exec/timed_thread_scheduler.hpp',
  meson.project_source_root() / 'include/exec/trampoline_scheduler.hpp',
  meson.project_source_root() / 'include/exec/variant_sender.hpp',
  meson.project_source_root() / 'include/exec/when_any.hpp',
  meson.project_source_root() / 'include/stdexec/concepts.hpp',
  meson.project_source_root() / 'include/stdexec/coroutine.hpp',
  meson.project_source_root() / 'include/stdexec/execution.hpp',
  meson.project_source_root() / 'include/stdexec/functional.hpp',
  meson.project_source_root() / 'include/stdexec/stop_token.hpp',
  meson.project_source_root() / 'include/tbbexec/tbb_thread_pool.hpp',
]

#doc_conf_data = configuration_data()
#doc_conf_data.set('DOXYGEN_INPUT_DIR', meson.project_source_root() / 'include')
#doc_conf_data.set('DOXYGEN_OUTPUT_DIR', meson.project_build_root() / 'doxygen')
#doxygen_index_file = meson.project_build_root() / 'doxygen' / 'xml' / 'index.xml'
#doc_conf_data.set('DOXYGEN_INDEX_FILE', doxygen_index_file)

#doxyfile = configure_file(input: 'docs/Doxyfile.in',
#                          output: '@BASENAME@',
#                          configuration: doc_conf_data)

#tag_file = custom_target(output: 'doxygen',
#                         input: doxyfile,
#                         command: [doxygen, '@INPUT@'],
#                         depend_files: stdexec_public_headers,
#                         build_by_default: true)

#sphinx = find_program('sphinx-build')

#tag_file = custom_target(output: 'sphinx-docs',
#                         input: doxyfile,
#                         command: [sphinx, '-b', 'html',
#                                   '-Dbreathe_projects.stdexec=' + meson.project_build_root() / 'doxygen' / 'xml',
#                                   meson.project_source_root() / 'docs' / 'source',
#                                   meson.project_build_root() / 'sphinx-docs'],
#                         depends: tag_file,
#                         build_by_default: true)
