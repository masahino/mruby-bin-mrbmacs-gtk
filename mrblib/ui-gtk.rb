# coding: utf-8
module Mrbmacs
  class Frame
    include Scintilla
    attr_accessor :view_win, :echo_win, :tk, :mainwin, :edit_win

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

    def send_key(key, win = nil)
    end

    def modeline(app)
      mode_text = get_mode_str(app)
      set_mode_text(mode_text)
      #      @mode_win.set_text(mode_text)
    end

    def modeline_refresh(app)
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

    def waitkey(win)
      if @key_buffer.size > 0
        @key_buffer.pop
      else
        false
      end
    end
    
    def exit
    end
  end
end
