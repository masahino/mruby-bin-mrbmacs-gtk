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
    end

    def set_theme(theme)
      set_theme_base(theme)
      @sci.sci_set_fold_margin_colour(true, theme.background_color)
      @sci.sci_set_fold_margin_hicolour(true, theme.foreground_color)
      for n in 25..31
        @sci.sci_marker_set_fore(n, theme.foreground_color)
        @sci.sci_marker_set_back(n, theme.background_color)
      end
    end
  end
end
