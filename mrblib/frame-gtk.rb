module Mrbmacs
  class Frame
    attr_accessor :mainwin

    def initialize(buffer)
      frame_gtk_init(buffer, 80, 40)
      #      set_style_gtk
      #      @edit_win.set_sci_default
      #      @edit_win.set_margin
      @sci_notifications = [] # for compatible
      setup_echo_win
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
      apply_echo_theme(theme) unless @echo_win.nil?
    end

    def apply_echo_theme(theme)
      @echo_win.sci_style_set_fore(Scintilla::STYLE_DEFAULT, theme.foreground_color)
      @echo_win.sci_style_set_back(Scintilla::STYLE_DEFAULT, theme.background_color)
      @echo_win.sci_style_clear_all
      # sci_style_clear_all resets STYLE_LINENUMBER.back to the light system
      # colour; the prompt lives in an SC_MARGIN_TEXT margin filled with it.
      @echo_win.sci_style_set_fore(Scintilla::STYLE_LINENUMBER, theme.foreground_color)
      @echo_win.sci_style_set_back(Scintilla::STYLE_LINENUMBER, theme.background_color)
      @echo_win.sci_set_caret_fore(theme.foreground_color)
    end

    def echo_puts(text)
      return if @echo_win.nil?

      msg = text.to_s
      @echo_win.sci_clear_all
      @echo_win.sci_add_text(msg.bytesize, msg)
      @echo_win.sci_document_end
    end

    def echo_set_prompt(prompt)
      width = @echo_win.sci_text_width(Scintilla::STYLE_DEFAULT, prompt)
      @echo_win.sci_set_margin_widthn(3, width)
      @echo_win.sci_margin_set_text(0, prompt)
    end

    def echo_gets(prompt, text = '', &block)
      @echo_win.sci_clear_all
      echo_set_prompt(prompt)
      @echo_win.sci_add_text(text.bytesize, text)
      input = nil

      loop do
        case wait_echo_event
        when :cancel
          break
        when :enter
          if @echo_win.sci_autoc_active
            @echo_win.sci_autoc_complete
          else
            input = @echo_win.sci_get_line(0)
            break
          end
        when :tab
          complete_echo_input(block) unless block.nil?
        end
      end
      input
    ensure
      @echo_win.sci_autoc_cancel unless @echo_win.nil?
      echo_set_prompt('') unless @echo_win.nil?
      @echo_win.sci_add_text(1, ' ') unless @echo_win.nil?
      @echo_win.sci_clear_all unless @echo_win.nil?
      @view_win.sci_grab_focus
    end

    def complete_echo_input(block)
      input_text = @echo_win.sci_get_line(0)
      was_active = @echo_win.sci_autoc_active
      @echo_win.sci_autoc_cancel if was_active
      completion_list, length = block.call(input_text)

      if was_active
        candidates = completion_list.split(@echo_win.sci_autoc_get_separator.chr)
        common = Mrbmacs.common_prefix(candidates)
        unless common.nil?
          suffix = common[length..]
          @echo_win.sci_add_text(suffix.bytesize, suffix) unless suffix.nil?
          length = common.length
        end
      end
      @echo_win.sci_autoc_show(length, completion_list)
    end

    def select_buffer(default_buffername, buffer_list)
      prompt = "Switch to buffer: (default #{default_buffername}) "
      echo_gets(prompt, '') do |input_text|
        candidates = buffer_list.select do |name|
          name[0, input_text.length] == input_text
        end
        [
          candidates.join(@echo_win.sci_autoc_get_separator.chr),
          input_text.length
        ]
      end
    end

    def y_or_n(prompt)
      @echo_win.sci_clear_all
      echo_set_prompt(prompt)
      wait_confirmation_event == :yes
    ensure
      @echo_win.sci_clear_all unless @echo_win.nil?
      echo_set_prompt('') unless @echo_win.nil?
      @view_win.sci_grab_focus
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
