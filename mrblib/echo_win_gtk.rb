module Mrbmacs
  class Frame
    # Configure the echo-area Scintilla widget as a one-line minibuffer.
    # The widget itself is created in scintilla_echo_window_new (C); this sets
    # up the parts shared with the other frontends' echo windows.
    def setup_echo_win
      @echo_win.sci_set_hscrollbar(false)
      @echo_win.sci_set_vscrollbar(false)
      @echo_win.sci_autoc_set_choose_single(1)
      @echo_win.sci_set_caret_style(
        Scintilla::CARETSTYLE_BLOCK_AFTER |
        Scintilla::CARETSTYLE_OVERSTRIKE_BLOCK |
        Scintilla::CARETSTYLE_BLOCK
      )
      (0..2).each { |margin| @echo_win.sci_set_margin_widthn(margin, 0) }
      @echo_win.sci_set_margin_typen(3, Scintilla::SC_MARGIN_TEXT)
    end
  end
end
