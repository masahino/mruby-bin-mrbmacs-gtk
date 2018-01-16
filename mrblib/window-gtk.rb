# coding: utf-8
module Mrbmacs
  class EditWindow
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
  end
end