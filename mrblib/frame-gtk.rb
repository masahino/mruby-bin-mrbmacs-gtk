module Mrbmacs
  class Frame
    # Initial editor grid, shared with the Cocoa frontend so the two GUI
    # frontends open at the same size (landscape, ~1.4:1).
    INITIAL_COLUMNS = 120
    INITIAL_LINES = 40

    attr_accessor :mainwin

    def initialize(buffer)
      frame_gtk_init(buffer, INITIAL_COLUMNS, INITIAL_LINES)
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

    def echo_puts(text)
      return if @echo_win.nil?

      msg = text.to_s
      @echo_win.sci_clear_all
      @echo_win.sci_add_text(msg.bytesize, msg)
      @echo_win.sci_document_end
    end

    # --- non-modal echo prompts (isearch / query-replace) ---------------
    # The pattern is edited directly in @echo_win while the frame is in
    # echo_key_mode; control keys go through ApplicationGtk#echo_key.

    def start_isearch(prompt)
      @echo_win.sci_clear_all
      echo_set_prompt(prompt)
      @echo_win.sci_grab_focus
      echo_key_mode(true)
    end

    def update_isearch_prompt(prompt)
      echo_set_prompt(prompt)
    end

    def set_isearch_text(text)
      @echo_win.sci_clear_all
      @echo_win.sci_add_text(text.bytesize, text)
      @echo_win.sci_document_end
    end

    def finish_isearch
      echo_key_mode(false)
      @echo_win.sci_clear_all
      echo_set_prompt('')
      @echo_win.sci_add_text(1, ' ')
      @echo_win.sci_clear_all
      @view_win.sci_grab_focus
    end

    def start_query_replace(prompt)
      @echo_win.sci_clear_all
      echo_set_prompt(prompt)
      @echo_win.sci_grab_focus
      echo_key_mode(true)
    end

    def finish_query_replace
      echo_key_mode(false)
      @echo_win.sci_clear_all
      echo_set_prompt('')
      @echo_win.sci_add_text(1, ' ')
      @echo_win.sci_clear_all
      @view_win.sci_grab_focus
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
