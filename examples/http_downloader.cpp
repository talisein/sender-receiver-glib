#include <span>
#include <iostream>
#include <gtkmm.h>
#include <glibmm.h>
#include <curl/curl.h>
#include "gtkmmstdexec.hpp"
#include <exec/async_scope.hpp>

extern "C" {
    size_t writefunction_wrap(char *ptr, size_t size, size_t nmemb, void *userdata);
}

struct curler
{
    CURL* easy;
    std::function<size_t (std::span<char>)> write_function;

    curler(const curler&) = delete;
    curler(curler&& in) noexcept :
        easy(in.easy),
        write_function(std::move(in.write_function))
    {
        in.easy = nullptr;
        curl_easy_setopt(easy, CURLOPT_WRITEDATA, this);
        std::cout << "curl movement!\n";
    }

    curler() noexcept :
        easy(curl_easy_init()),
        write_function()
    {
        std::cout << "curl construction_\n";
        curl_easy_setopt(easy, CURLOPT_WRITEDATA, this);
        curl_easy_setopt(easy, CURLOPT_WRITEFUNCTION, writefunction_wrap);
    }

    void set_url(const char* url) {
        curl_easy_setopt(easy, CURLOPT_URL, url);
    }

    template <typename Func>
    void set_write_function(Func&& f) { write_function = std::forward<Func>(f); };

    ~curler() { if (easy != nullptr) {std::cout << "curl destruction!\n"; curl_easy_cleanup(easy);}}
};


extern "C" {
    size_t writefunction_wrap(char *ptr, size_t size, size_t nmemb, void *userdata) {
        return static_cast<curler*>(userdata)->write_function(std::span(ptr, size * nmemb));
    }
}

class HelloWorld : public Gtk::Window
{
public:
    HelloWorld() :
        m_worker_context(Glib::MainContext::create()),
        m_worker_mainloop(Glib::MainLoop::create(m_worker_context)),
        m_worker_thread([this] { worker_thread_start(); }),
        GUIScheduler(Glib::MainContext::get_default()),
        ThreadScheduler(m_worker_context),
        m_grid(),
        m_button_blocking("Blocking"),
        m_button_stdexec("stdexec"),
        m_mediafile(Gtk::MediaFile::create_for_resource("/org/gtkmm/example/hatsune-miku.mp4")),
        m_picture(m_mediafile)
    {
        // Sets the margin around the button.
        m_button_blocking.set_margin(10);
        m_button_stdexec.set_margin(10);

        m_mediafile->set_loop(true);
        m_mediafile->play();

        // When the button receives the "clicked" signal, it will call the
        // on_button_clicked() method defined below.
        m_button_blocking.signal_clicked().connect(sigc::mem_fun(*this,
                                                                 &HelloWorld::on_button_blocking_clicked));
        m_button_stdexec.signal_clicked().connect(sigc::mem_fun(*this,
                                                                &HelloWorld::on_button_stdexec_clicked));
        // This packs the button into the Window (a container).
        m_grid.attach(m_button_blocking,0,0);
        m_grid.attach(m_button_stdexec,0,1);
        m_grid.attach(m_picture,1,0,1,2);
        set_child(m_grid);
    }
    ~HelloWorld() override { worker_thread_stop(); };
private:
    void worker_thread_start() {
        m_worker_context->push_thread_default();
        m_worker_mainloop->run();
    }
    void worker_thread_stop() {
        m_worker_context->invoke([this] {m_worker_mainloop->quit(); return false;});
        m_worker_thread.join();
    }

    //Signal handlers:
    void
    on_button_blocking_clicked()
    {
        m_button_blocking.set_sensitive(false);
        m_button_blocking.set_label("Blocking...");
        curler curl;
        curl.set_write_function([this, downloaded = 0UZ] (auto buf) mutable
        {
            downloaded += buf.size_bytes();
            m_button_blocking.set_label(Glib::ustring::compose("Blocking (%1 bytes so far)", downloaded));
            return buf.size_bytes();
        });
        curl.set_url("https://ftp.funet.fi/pub/Linux/kernel/v5.x/linux-5.19.tar.gz");
        const auto code = curl_easy_perform(curl.easy);
        if (CURLE_OK == code) {
            curl_off_t size = 0;
            curl_easy_getinfo(curl.easy, CURLINFO_SIZE_DOWNLOAD_T, &size);
            std::cout << "Blocking downloaded " << size << " bytes\n";
            m_button_blocking.set_label(Glib::ustring::compose("Blocking (%1 bytes OK)", size));
        } else {
            std::cerr << "Blocking error: " << curl_easy_strerror(code) << "\n";
            m_button_blocking.set_label(Glib::ustring::compose("Blocking (Error: %1)", curl_easy_strerror(code)));
        }
        m_button_blocking.set_sensitive(true);
    }

    void
    on_button_stdexec_clicked()
    {
        m_button_stdexec.set_sensitive(false);
        m_button_stdexec.set_label("Blocking?");
        curler curl {};
        curl.set_url("https://ftp.funet.fi/pub/Linux/kernel/v5.x/linux-5.19.tar.gz");
        curl.set_write_function([this, downloaded = 0UZ] (auto buf) mutable
        {
            std::cout << "stdexec: Progress. Worker thread\n";
            downloaded += buf.size_bytes();
            auto updateProgressSender = stdexec::just(downloaded)
                | stdexec::then([this](size_t total) {
                    std::cout << "stdexec: Progress. GUI thread, setting label\n";
                    m_button_stdexec.set_label(Glib::ustring::compose("stdexec (%1 bytes so far)", total));
                });

            std::cout << "stdexec: calling async_scope::spawn on updateProgressSender...\n";
            async_scope.spawn(stdexec::on(GUIScheduler, std::move(updateProgressSender)));
            return buf.size_bytes();
        });


        auto downloadSender = stdexec::just(1)
            | stdexec::then([c = std::move(curl)] (int i) {
                (void)i;
                std::cout << "stdexec: Starting curl transfer\n";
                auto code = curl_easy_perform(c.easy);
                if (code != CURLE_OK) {
                    std::cout << "Got curl error " << curl_easy_strerror(code) << ". Throwing...\n";
                    throw code;
                }
                curl_off_t size = 0;
                curl_easy_getinfo(c.easy, CURLINFO_SIZE_DOWNLOAD_T, &size);
                std::cout << "stdexec: transfer finished, changing threads...\n";
                return size;
            })
            | stdexec::continues_on(GUIScheduler)
            | stdexec::then([this](curl_off_t size) {
                std::cout << "stdexec: Back in GUI thread, reporting finish.\n";
                m_button_stdexec.set_sensitive(true);
                m_button_stdexec.set_label(Glib::ustring::compose("stdexec (%1 bytes OK)", size));
            })
            | stdexec::upon_error([this](std::exception_ptr p) {
                std::cout << "In error path\n";
                try {
                    std::rethrow_exception(p);
                } catch (CURLcode code) {
                    m_button_stdexec.set_label(Glib::ustring::compose("stdexec (Error: %1)", curl_easy_strerror(code)));
                } catch (std::exception& e) {
                    std::cout << "exception is " << e.what() << "\n";
                }
                std::cout << "Error path ends.\n";
            });
        std::cout << "stdexec: calling async_scope::spawn on downloadSender...\n";
        async_scope.spawn(stdexec::on(ThreadScheduler, std::move(downloadSender)));
    }

    //Member widgets:
    Glib::RefPtr<Glib::MainContext> m_worker_context;
    Glib::RefPtr<Glib::MainLoop> m_worker_mainloop;
    std::thread m_worker_thread;
    GlibExec::Scheduler GUIScheduler;
    GlibExec::Scheduler ThreadScheduler;
    exec::async_scope async_scope;
    Gtk::Grid m_grid;
    Gtk::Button m_button_blocking;
    Gtk::Button m_button_stdexec;
    Glib::RefPtr<Gtk::MediaFile> m_mediafile;
    Gtk::Picture m_picture;
};

int main(int argc, char *argv[])
{
  auto app = Gtk::Application::create("org.gtkmm.example");

  return app->make_window_and_run<HelloWorld>(argc, argv);
}
