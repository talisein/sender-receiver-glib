#pragma once

#include <iostream>
#include <stdexec/concepts.hpp>
#include <stdexec/execution.hpp>
#include <glibmm.h>

namespace GlibExec
{

    template <class Recv>
    class MainContextOperationState;

    class Scheduler
    {
    public:
        explicit Scheduler(const Glib::RefPtr<Glib::MainContext>& ctx) :
            m_context(ctx)
        {
        }

        Glib::RefPtr<Glib::MainContext>
        get_context()
        {
            return m_context;
        }

        struct default_env {
            Glib::RefPtr<Glib::MainContext> context;

            template <typename T>
            friend Scheduler
            tag_invoke(stdexec::get_completion_scheduler_t<T>, default_env env) noexcept
            {
                return Scheduler(env.context);
            }
        };

        class MainContextSender
        {
        public:
            using is_sender = void;
            using completion_signatures = stdexec::completion_signatures<stdexec::set_value_t(), stdexec::set_error_t(std::exception_ptr)>;

            explicit MainContextSender(Glib::RefPtr<Glib::MainContext> ctx) :
                m_context(std::move(ctx))
            {
            }

            Glib::RefPtr<Glib::MainContext>
            get_context()
            {
                return m_context;
            }

            friend default_env
            tag_invoke(stdexec::get_env_t, const MainContextSender& snd) noexcept
            {
                return {snd.m_context};
            }

            template <class Recv>
            friend inline MainContextOperationState<Recv>
            tag_invoke(stdexec::connect_t, MainContextSender sender, Recv&& receiver)
            {
                return MainContextOperationState<Recv>(std::move(receiver), sender.get_context());
            }

        private:
            Glib::RefPtr<Glib::MainContext> m_context;
        };

        friend MainContextSender
        tag_invoke(stdexec::schedule_t, Scheduler sched)
        {
            return MainContextSender(sched.get_context());
        }

        friend bool
        operator==(const Scheduler& a, const Scheduler& b) noexcept
        {
            return a.m_context->gobj() == b.m_context->gobj();
        }

        friend bool
        operator!=(const Scheduler& a, const Scheduler& b) noexcept
        {
            return a.m_context->gobj() != b.m_context->gobj();
        }


    private:
        Glib::RefPtr<Glib::MainContext> m_context;
    };

    template <class Recv>
    class MainContextOperationState
    {
    public:
        MainContextOperationState(Recv&& receiver, const Glib::RefPtr<Glib::MainContext>& ctx) :
            m_receiver(std::move(receiver)),
            m_context(ctx)
        {
            puts("MainContextOperationState()");
        }
        ~MainContextOperationState()
        {
            puts("~MainContextOperationState()");
        }
        MainContextOperationState(const MainContextOperationState&) = delete;
        MainContextOperationState(MainContextOperationState&&) = delete;

        void
        start() noexcept
        {
            m_context->invoke([this] { stdexec::set_value(std::move(m_receiver)); return false; });
        }

        friend void
        tag_invoke(stdexec::tag_t<stdexec::start>, MainContextOperationState& state) noexcept
        {
            state.start();
        }

    private:
        Recv m_receiver;
        Glib::RefPtr<Glib::MainContext> m_context;
    };

    template <template<class> class Connectable, class Recv, class Ret, class... Args>
    class GObjectOperationState;

    template <template<class> class Connectable, class Ret, class... Args>
    class GObjectSender
    {
        struct default_env {
            Glib::RefPtr<Glib::MainContext> context;

            template <typename T>
            friend Scheduler
            tag_invoke(stdexec::get_completion_scheduler_t<T>, default_env env) noexcept
            {
                return Scheduler(env.context);
            }
        };

        friend default_env
        tag_invoke(stdexec::get_env_t, const GObjectSender& snd) noexcept
        {
            return {Glib::MainContext::get_default()};
        }

    public:
        using is_sender = void;
        using completion_signatures = stdexec::completion_signatures<
            stdexec::set_value_t(Args...),
            stdexec::set_error_t(std::exception_ptr)>;

        GObjectSender(Connectable<Ret(Args...)>&& connectable) :
            m_connectable(std::forward<Connectable<Ret(Args...)>>(connectable))
        {
        }
        Connectable<Ret(Args...)> get_connectable() { return m_connectable; };

        template <class Recv>
        friend inline GObjectOperationState<Connectable, Recv, Ret, Args...>
        tag_invoke(stdexec::tag_t<stdexec::connect>, GObjectSender sender, Recv&& receiver)
        {
            // TODO: move the connectable?
            return GObjectOperationState<Connectable, Recv, Ret, Args...>(std::move(receiver), sender.m_connectable);
        }

    private:
        Connectable<Ret(Args...)> m_connectable;
    };


    template <template<class> class Connectable, class Recv, class Ret, class... Args>
    class GObjectOperationState
    {
    public:
        GObjectOperationState(Recv&& receiver, Connectable<Ret(Args...)>&& connectable)
            : m_receiver(std::move(receiver)),
              m_connectable(std::forward<Connectable<Ret(Args...)>>(connectable))
        {
        }

        friend void tag_invoke(stdexec::tag_t<stdexec::start>, GObjectOperationState& oper_state) noexcept
        {
            if constexpr (std::is_same_v<Ret, void>) {
                oper_state.connection = oper_state.m_connectable.connect([&oper_state](Args... args) {
                    stdexec::set_value(std::move(oper_state.m_receiver), std::forward<Args>(args)...);
                });
            } else {
                oper_state.connection = oper_state.m_connectable.connect_notify([&oper_state](Args... args) {
                    stdexec::set_value(std::move(oper_state.m_receiver), std::forward<Args>(args)...);
                });
            }
        }

    private:
        Recv m_receiver;
        Connectable<Ret(Args...)> m_connectable;
        sigc::scoped_connection connection;
    };

// I want to:
// gObjSigAsSender(button.signal_clicked())
// button.signal_clicked() returns a Glib::SignalProxy<void()>
    template <template<class> class Connectable, class Ret, class... Args>
    inline GObjectSender<Connectable, Ret, Args...>
    GObjectAsSender(Connectable<Ret(Args...)>&& connectable)
    {
        return GObjectSender<Connectable, Ret, Args...>(std::forward<Connectable<Ret(Args...)>>(connectable));
    }
}
