# coding: utf-8
module Mrbmacs
  class EditWindowGtk < EditWindow
    attr_accessor :sci, :frame
    attr_accessor :buffer, :x1, :y1, :width, :height

    def initialize(frame, buffer, x1, y1, width, height)
      @frame = frame
      @buffer = buffer
      @x1 = x1
      @y1 = y1
      @width = width
      @height = height
      @sci = Scintilla::ScintillaGtk.new
      set_callback
      setup_sci_default
      compute_area
      set_style_gtk
      set_margin
      if @theme != nil
        set_theme(@theme)
      end
    end

    def setup_sci_default
      font = "Monospace"
      if Scintilla::PLATFORM == :GTK_MACOSX
        font = "Monaco"
      end
      size = 14
      set_font(font, size)

      @sci.sci_set_caret_fore 0xffffff
      @sci.sci_set_caretstyle(Scintilla::CARETSTYLE_BLOCK_AFTER | Scintilla::CARETSTYLE_OVERSTRIKE_BLOCK | Scintilla::CARETSTYLE_BLOCK)
      @sci.sci_set_mouse_dwell_time 1000
    end

   def set_style_gtk
      @sci.sci_marker_define(Scintilla::SC_MARKNUM_FOLDEROPEN, Scintilla::SC_MARK_BOXMINUS)
      @sci.sci_marker_set_fore(Scintilla::SC_MARKNUM_FOLDEROPEN, 0xffffff)
      @sci.sci_marker_set_back(Scintilla::SC_MARKNUM_FOLDEROPEN, 0x000000)
      @sci.sci_marker_define(Scintilla::SC_MARKNUM_FOLDER, Scintilla::SC_MARK_BOXPLUS)
      @sci.sci_marker_set_fore(Scintilla::SC_MARKNUM_FOLDER, 0xffffff)
      @sci.sci_marker_set_back(Scintilla::SC_MARKNUM_FOLDER, 0x000000)
      @sci.sci_marker_define(Scintilla::SC_MARKNUM_FOLDERSUB, Scintilla::SC_MARK_VLINE)
      @sci.sci_marker_set_fore(Scintilla::SC_MARKNUM_FOLDERSUB, 0xffffff)
      @sci.sci_marker_set_back(Scintilla::SC_MARKNUM_FOLDERSUB, 0x000000)
      @sci.sci_marker_define(Scintilla::SC_MARKNUM_FOLDERTAIL, Scintilla::SC_MARK_LCORNER)
      @sci.sci_marker_set_fore(Scintilla::SC_MARKNUM_FOLDERTAIL, 0xffffff)
      @sci.sci_marker_set_back(Scintilla::SC_MARKNUM_FOLDERTAIL, 0x000000)
      @sci.sci_marker_define(Scintilla::SC_MARKNUM_FOLDEREND, Scintilla::SC_MARK_BOXPLUSCONNECTED)
      @sci.sci_marker_set_fore(Scintilla::SC_MARKNUM_FOLDEREND, 0xffffff)
      @sci.sci_marker_set_back(Scintilla::SC_MARKNUM_FOLDEREND, 0x000000)
      @sci.sci_marker_define(Scintilla::SC_MARKNUM_FOLDEROPENMID, Scintilla::SC_MARK_BOXMINUSCONNECTED)
      @sci.sci_marker_set_fore(Scintilla::SC_MARKNUM_FOLDEROPENMID, 0xffffff)
      @sci.sci_marker_set_back(Scintilla::SC_MARKNUM_FOLDEROPENMID, 0x000000)
      @sci.sci_marker_define(Scintilla::SC_MARKNUM_FOLDERMIDTAIL, Scintilla::SC_MARK_TCORNER)
      @sci.sci_marker_set_fore(Scintilla::SC_MARKNUM_FOLDERMIDTAIL, 0xffffff)
      @sci.sci_marker_set_back(Scintilla::SC_MARKNUM_FOLDERMIDTAIL, 0x000000)
      @sci.sci_set_indentation_guides(Scintilla::SC_IV_LOOKBOTH)
    end

    def set_theme(theme)
      @theme = theme
      set_theme_base(theme)
      @sci.sci_set_fold_margin_colour(true, theme.background_color)
      @sci.sci_set_fold_margin_hicolour(true, theme.foreground_color)
      for n in 25..31
        @sci.sci_marker_set_fore(n, theme.foreground_color)
        @sci.sci_marker_set_back(n, theme.background_color)
      end
    end

    def set_font(font, size)
      @sci.sci_style_set_font Scintilla::STYLE_DEFAULT, font
      @sci.sci_style_set_size Scintilla::STYLE_DEFAULT, size
      font_width = @sci.sci_text_width Scintilla::STYLE_DEFAULT, "A"
      font_height = @sci.sci_text_height 1
    end
  end
end
