#include <gtkmm.h>

void on_activate(const Glib::RefPtr<Gtk::Application>& app)
{
    // Crear ventana asociada a la app
    auto window = Gtk::make_managed<Gtk::ApplicationWindow>(app);
    window->set_title("NoteThat");
    window->set_default_size(800, 600);

    // Área de texto
    auto text_view = Gtk::make_managed<Gtk::TextView>();
    text_view->set_wrap_mode(Gtk::WrapMode::WORD);

    window->set_child(*text_view);
    window->present();
}

int main(int argc, char* argv[])
{
    auto app = Gtk::Application::create("com.notethat.app");

    // Conectar señal activate
    app->signal_activate().connect(
        sigc::bind(sigc::ptr_fun(&on_activate), app)
    );

    return app->run();
}
