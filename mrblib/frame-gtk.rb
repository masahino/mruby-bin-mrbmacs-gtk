# coding: utf-8
module Mrbmacs
  class Frame
    include Scintilla
    attr_accessor :view_win, :echo_win, :tk, :mainwin, :edit_win

    def initialize(buffer)
      frame_gtk_init(buffer)
#      set_style_gtk
#      @edit_win.set_sci_default
#      @edit_win.set_margin
    end

    def set_style_gtk
      @view_win.sci_marker_define(SC_MARKNUM_FOLDEROPEN, SC_MARK_BOXMINUS)
      @view_win.sci_marker_set_fore(SC_MARKNUM_FOLDEROPEN, 0xffffff)
      @view_win.sci_marker_set_back(SC_MARKNUM_FOLDEROPEN, 0x000000)
      @view_win.sci_marker_define(SC_MARKNUM_FOLDER, SC_MARK_BOXPLUS)
      @view_win.sci_marker_set_fore(SC_MARKNUM_FOLDER, 0xffffff)
      @view_win.sci_marker_set_back(SC_MARKNUM_FOLDER, 0x000000)
      @view_win.sci_marker_define(SC_MARKNUM_FOLDERSUB, SC_MARK_VLINE)
      @view_win.sci_marker_set_fore(SC_MARKNUM_FOLDERSUB, 0xffffff)
      @view_win.sci_marker_set_back(SC_MARKNUM_FOLDERSUB, 0x000000)
      @view_win.sci_marker_define(SC_MARKNUM_FOLDERTAIL, SC_MARK_LCORNER)
      @view_win.sci_marker_set_fore(SC_MARKNUM_FOLDERTAIL, 0xffffff)
      @view_win.sci_marker_set_back(SC_MARKNUM_FOLDERTAIL, 0x000000)
      @view_win.sci_marker_define(SC_MARKNUM_FOLDEREND, SC_MARK_BOXPLUSCONNECTED)
      @view_win.sci_marker_set_fore(SC_MARKNUM_FOLDEREND, 0xffffff)
      @view_win.sci_marker_set_back(SC_MARKNUM_FOLDEREND, 0x000000)
      @view_win.sci_marker_define(SC_MARKNUM_FOLDEROPENMID, SC_MARK_BOXMINUSCONNECTED)
      @view_win.sci_marker_set_fore(SC_MARKNUM_FOLDEROPENMID, 0xffffff)
      @view_win.sci_marker_set_back(SC_MARKNUM_FOLDEROPENMID, 0x000000)
      @view_win.sci_marker_define(SC_MARKNUM_FOLDERMIDTAIL, SC_MARK_TCORNER)
      @view_win.sci_marker_set_fore(SC_MARKNUM_FOLDERMIDTAIL, 0xffffff)
      @view_win.sci_marker_set_back(SC_MARKNUM_FOLDERMIDTAIL, 0x000000)
    end

    def set_theme(theme)
      @theme = theme
      @edit_win_list.each do |w|
        w.set_theme(theme)
      end
    end

    def select_buffer(default_buffer_name, buffer_list)
      select_item("select buffer", default_buffer_name, buffer_list)
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

    def waitkey(win)
      if @key_buffer.size > 0
        @key_buffer.pop
      else
        false
      end
    end

    def echo_puts(str)
    end

    def exit
    end
  end
end
