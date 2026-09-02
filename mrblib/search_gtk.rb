module Mrbmacs
  # Incremental search in the echo area, mirroring the Cocoa frontend
  # (search_cocoa.rb). The pattern is typed into @echo_win; text changes there
  # arrive through echo_sci_notify and drive the search, control keys (C-s /
  # C-r / Enter / C-g) arrive through echo_key while the frame is in
  # echo_key_mode. To be shared with Cocoa once both are proven.
  class ApplicationGtk < Application
    # Build a key string and dispatch it while echo owns the keyboard.
    # Returns true when mrbmacs consumed the key, false to let the character
    # reach @echo_win (i.e. type it into the pattern).
    def echo_key(state, keyval)
      key = case keyval
            when 0xff1b then 'Escape'
            when 0xff08, 0xffff then 'DEL'
            when 0xff0d, 0xff8d then 'Enter'
            when 0xff09 then 'Tab'
            when 0x20..0x7e then "#{get_modifier_string(state)}#{keyval.chr}"
            end
      return false if key.nil?

      echo_key_press(key)
    end

    def echo_key_press(key)
      return query_replace_key_press(key) if @query_replace_active
      return false unless @isearch_active

      case key
      when 'C-s'
        isearch_forward
      when 'C-r'
        isearch_backward
      when 'Enter'
        finish_isearch(false)
      when 'C-g'
        finish_isearch(true)
      else
        if key.start_with?('C-', 'M-') || key == 'Escape'
          finish_isearch(false)
        else
          return false
        end
      end
      true
    end

    def echo_sci_notify(_notification)
      return unless @isearch_active
      return if @isearch_setting_text

      text = @frame.echo_win.sci_get_line(0)
      return if text == @isearch_text

      @isearch_text = text
      if text.empty?
        search_highlight_begin('')
        @frame.view_win.sci_goto_pos(@isearch_origin)
        @frame.modeline(self)
        return
      end
      @last_search_text = text unless text.empty?
      perform_isearch(@isearch_origin)
    end

    def isearch_active?
      @isearch_active
    end

    def isearch_forward
      start_or_repeat_isearch(false)
    end

    def isearch_backward
      start_or_repeat_isearch(true)
    end

    def start_or_repeat_isearch(backward)
      if @isearch_active
        @isearch_backward = backward
        @frame.update_isearch_prompt(isearch_prompt)
        repeat_isearch
        return
      end

      @isearch_active = true
      @isearch_backward = backward
      @isearch_origin = @frame.view_win.sci_get_current_pos
      @isearch_text = ''
      @frame.start_isearch(isearch_prompt)
    end

    def repeat_isearch
      if @isearch_text.empty?
        return if @last_search_text.nil? || @last_search_text.empty?

        @isearch_text = @last_search_text
        begin
          @isearch_setting_text = true
          @frame.set_isearch_text(@isearch_text)
          @frame.update_isearch_prompt(isearch_prompt)
        ensure
          @isearch_setting_text = false
        end
      end
      view = @frame.view_win
      start_pos = @isearch_backward ? view.sci_get_selection_start : view.sci_get_selection_end
      perform_isearch(start_pos, true)
    end

    def perform_isearch(start_pos, wrap = false)
      return if @isearch_text.empty?

      view = @frame.view_win
      end_pos = @isearch_backward ? 0 : view.sci_get_length
      view.sci_set_target_start(start_pos)
      view.sci_set_target_end(end_pos)
      found = view.sci_search_in_target(@isearch_text.bytesize, @isearch_text)
      if found == -1 && wrap
        view.sci_set_target_start(@isearch_backward ? view.sci_get_length : 0)
        view.sci_set_target_end(@isearch_backward ? 0 : view.sci_get_length)
        found = view.sci_search_in_target(@isearch_text.bytesize, @isearch_text)
      end
      if found == -1
        search_highlight_begin(@isearch_text)
        @frame.modeline(self)
        return
      end

      view.sci_set_sel(view.sci_get_target_start, view.sci_get_target_end)
      search_highlight_begin(@isearch_text)
      @frame.modeline(self)
    end

    def finish_isearch(cancel)
      search_highlight_end
      @frame.view_win.sci_goto_pos(@isearch_origin) if cancel
      @isearch_active = false
      @frame.finish_isearch
      @frame.modeline(self)
    end

    def isearch_prompt
      @isearch_backward ? 'I-search backward: ' : 'I-search: '
    end
  end
end
