module Mrbmacs
  class Frame
    attr_accessor :mainwin

    def initialize(buffer)
      frame_gtk_init(buffer, 80, 40)
      #      set_style_gtk
      #      @edit_win.set_sci_default
      #      @edit_win.set_margin
      @sci_notifications = [] # for compatible
    end

    def new_editwin(buffer, left, top, width, height)
      EditWindowGtk.new(self, buffer, left, top, width, height)
    end

    def set_style_gtk
      @view_win.sci_marker_define(Scintilla::SC_MARKNUM_FOLDEROPEN, Scintilla::SC_MARK_BOXMINUS)
      @view_win.sci_marker_set_fore(Scintilla::SC_MARKNUM_FOLDEROPEN, 0xffffff)
      @view_win.sci_marker_set_back(Scintilla::SC_MARKNUM_FOLDEROPEN, 0x000000)
      @view_win.sci_marker_define(Scintilla::SC_MARKNUM_FOLDER, Scintilla::SC_MARK_BOXPLUS)
      @view_win.sci_marker_set_fore(Scintilla::SC_MARKNUM_FOLDER, 0xffffff)
      @view_win.sci_marker_set_back(Scintilla::SC_MARKNUM_FOLDER, 0x000000)
      @view_win.sci_marker_define(Scintilla::SC_MARKNUM_FOLDERSUB, Scintilla::SC_MARK_VLINE)
      @view_win.sci_marker_set_fore(Scintilla::SC_MARKNUM_FOLDERSUB, 0xffffff)
      @view_win.sci_marker_set_back(Scintilla::SC_MARKNUM_FOLDERSUB, 0x000000)
      @view_win.sci_marker_define(Scintilla::SC_MARKNUM_FOLDERTAIL, Scintilla::SC_MARK_LCORNER)
      @view_win.sci_marker_set_fore(Scintilla::SC_MARKNUM_FOLDERTAIL, 0xffffff)
      @view_win.sci_marker_set_back(Scintilla::SC_MARKNUM_FOLDERTAIL, 0x000000)
      @view_win.sci_marker_define(Scintilla::SC_MARKNUM_FOLDEREND, Scintilla::SC_MARK_BOXPLUSCONNECTED)
      @view_win.sci_marker_set_fore(Scintilla::SC_MARKNUM_FOLDEREND, 0xffffff)
      @view_win.sci_marker_set_back(Scintilla::SC_MARKNUM_FOLDEREND, 0x000000)
      @view_win.sci_marker_define(Scintilla::SC_MARKNUM_FOLDEROPENMID, Scintilla::SC_MARK_BOXMINUSCONNECTED)
      @view_win.sci_marker_set_fore(Scintilla::SC_MARKNUM_FOLDEROPENMID, 0xffffff)
      @view_win.sci_marker_set_back(Scintilla::SC_MARKNUM_FOLDEROPENMID, 0x000000)
      @view_win.sci_marker_define(Scintilla::SC_MARKNUM_FOLDERMIDTAIL, Scintilla::SC_MARK_TCORNER)
      @view_win.sci_marker_set_fore(Scintilla::SC_MARKNUM_FOLDERMIDTAIL, 0xffffff)
      @view_win.sci_marker_set_back(Scintilla::SC_MARKNUM_FOLDERMIDTAIL, 0x000000)
    end

    def apply_theme(theme)
      @theme = theme
      @edit_win_list.each do |w|
        w.apply_theme(theme)
      end
    end

    def select_buffer(default_buffer_name, buffer_list)
      select_item('select buffer', default_buffer_name, buffer_list)
    end

    def send_key(key, win = nil)
    end

    def modeline(app)
      mode_text = get_mode_str(app)
      set_mode_text(mode_text)
      #      @mode_win.set_text(mode_text)
    end

    def modeline_refresh(app)
    end

    def read_buffername(prompt)
      echo_gets(prompt)
    end

    def waitkey(_win)
      if @key_buffer.size > 0
        @key_buffer.pop
      else
        false
      end
    end

    def set_font(font, size)
      @edit_win_list.each do |w|
        w.set_font(font, size)
        w.set_style_gtk
        w.apply_theme(@theme)
      end
    end

    def exit
    end
  end
end
