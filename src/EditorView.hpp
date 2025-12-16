#pragma once
#include <gtkmm.h>
#include <functional>

class EditorView
{
private:
    Glib::RefPtr<Gtk::TextBuffer> buffer;
    Gtk::TextView* textView;
    Gtk::ScrolledWindow* scrolled;
    
public:
    EditorView()
    {
        // Crear el buffer
        buffer = Gtk::TextBuffer::create();
        
        // Crear el TextView
        textView = Gtk::make_managed<Gtk::TextView>();
        textView->set_buffer(buffer);
        textView->set_wrap_mode(Gtk::WrapMode::WORD);
        
        // Crear el ScrolledWindow
        scrolled = Gtk::make_managed<Gtk::ScrolledWindow>();
        scrolled->set_child(*textView);
    }

    // Getters
    Gtk::Widget* GetWidget() { return scrolled; }
    Glib::RefPtr<Gtk::TextBuffer> GetBuffer() { return buffer; }
    
    // Conectar señal de cambio
    void ConnectOnChanged(std::function<void()> callback)
    {
        buffer->signal_changed().connect(callback);
    }
};