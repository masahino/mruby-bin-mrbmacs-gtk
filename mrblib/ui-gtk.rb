# coding: utf-8
module Mrbmacs
  class Frame
    include Scintilla
    attr_accessor :view_win, :echo_win, :tk, :mainwin

    def initialize
#      Gtk::set_locale
      Gtk::init
      @mainwin = Gtk::Window.new Gtk::WindowType::TOPLEVEL
      @vbox = Gtk::VBox.new(false, 2)
      @mainwin.add(@vbox)
      @keysyms = [0,
                  Scintilla::SCK_BACK,
                  Scintilla::SCK_TAB,
                  Scintilla::SCK_RETURN,
                  Scintilla::SCK_ESCAPE,
                  0,
                  Scintilla::SCK_BACK,
                  Scintilla::SCK_UP,
                  Scintilla::SCK_DOWN,
                  Scintilla::SCK_LEFT,
                  Scintilla::SCK_RIGHT,
                  0,0,
                  Scintilla::SCK_INSERT,
                  Scintilla::SCK_DELETE,
                  0,
                  Scintilla::SCK_PRIOR,
                  Scintilla::SCK_NEXT,
                  Scintilla::SCK_HOME,
                  Scintilla::SCK_END]


      @view_win = ScintillaGtk.new
      @view_win.sci_style_set_font(Scintilla::STYLE_DEFAULT, "Monospace")
      @view_win.sci_style_set_size(Scintilla::STYLE_DEFAULT, 14)
      font_width = @view_win.sci_text_width(Scintilla::STYLE_DEFAULT, "A")
      font_height = @view_win.sci_text_height(1)
#      Gtk::Widget.set_usize(@view_win.editor, font_width.to_i*(80+6), font_height.to_i*40)
      Gtk::Widget.set_size_request(@view_win.editor, font_width.to_i*(80+6), font_height.to_i*40)
      @vbox.pack_start(@view_win.editor, false, false, 0)
#      @mainwin.signal_connect("key-press-event") do |*o|
#        e = o[0].get_struct
#	$stderr.puts "keyval = #{e[:keyval]}, state = #{e[:state]}"
#        $stderr.puts "control? #{e[:state] & (1 << 2)}, meta? #{e[:state] & (1 << 28)}, mod1? #{e[:state] & (1 << 3)
#}, mod2? #{e[:state] & (1<<4)} mod3? #{e[:state] & (1<<5)}, mod4? #{e[:state] & (1<<6)}, mod5 #{e[:state] & (1<<7)}"
#      end
      @view_win.sci_set_caret_fore(0xffffff)
      @view_win.sci_set_caret_style(2)

      @mode_win = Gtk::Label.new
      @mode_win.set_justify(Gtk::Justification::LEFT)
      @vbox.pack_start(@mode_win, true, true, 0)

      @echo_win = ScintillaGtk.new
#      Gtk::Widget.set_usize(@echo_win.editor, font_width.to_i*80, font_height.to_i*1)
      Gtk::Widget.set_size_request(@echo_win.editor, font_width.to_i*80, font_height.to_i*1)
      @vbox.pack_start(@echo_win.editor, true, true, 0)

      @echo_win.sci_set_hscroll_bar(false)
      @echo_win.sci_clear_cmd_key(Scintilla::SCK_RETURN)

      @view_win.sci_set_focus(true)
      @echo_win.sci_set_focus(false)
      set_default_style
      
      @vbox.pack_start(Gtk::Statusbar.new, true, true, 0)

      @mainwin.show_all
      Gtk::Widget.grab_focus(@view_win.editor)
    end

    def send_key(key, win = nil)
    end

    def modeline(app)
      mode_text = get_mode_str(app)
      @mode_win.set_text(mode_text)
    end

    def echo_gets(prompt, text = "", &block)
      @view_win.sci_set_focus(false)
      @echo_win.sci_set_focus(true)
      Gtk::Widget.grab_focus(@echo_win.editor)
      @echo_win.sci_clear_all
      prefix_text = prompt + text
      @echo_win.sci_add_text(prefix_text.length, prefix_text)
      input_text = nil
#      while true
      #end
    end

    def read_buffername(prompt)
      echo_gets(prompt)
    end

    def exit
    end
  end
end
