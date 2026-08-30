module Mrbmacs
  # ApplicationGtk
  class ApplicationGtk < Application
    def add_buffer_to_frame(buffer)
      @frame.add_new_tab(buffer)
      @frame.view_win.sci_set_identifier(@frame.edit_win_list.index(@frame.edit_win))
    end

    def sci_notify(notify)
      $stderr.puts notify['code'] if $DEBUG
      call_sci_event(notify)
    end

    # Determines the modifier based on the state.
    def get_modifier_string(state)
      return 'C-' if (state & (1 << 2)) != 0
      return 'M-' if (state & (1 << 28)) != 0
      ''
    end

    # Determines the input string based on the key value.
    def get_input_string(keyval)
      case keyval
      when 0xff09 then 'Tab'
      when 0xff0d then 'Enter'
      when 0..255 then keyval.chr
      else ''
      end
    end

    def key_press(state, keyval)
      #      @frame.view_win.sci_set_empty_selection(@frame.view_win.sci_get_current_pos())
      send_key = true
      key_str = ''
      mod_str = get_modifier_string(state)
      input_str = get_input_string(keyval)

      return true if @frame.view_win.sci_get_focus == false && %w[Tab Enter].include?(input_str)

      if input_str != ''
        key_str = "#{@prefix_key}#{mod_str}#{input_str}"

        add_recent_key(mod_str + input_str)
        command = key_scan(key_str)
        if !command.nil?
          if command.is_a?(Integer)
            #            @frame.view_win.send_message(command, nil, nil)
            @prefix_key = ''
          end
          if command == 'prefix'
            @prefix_key = "#{key_str} "
          else
            extend(command)
            @prefix_key = ''
          end
          send_key = false
        else
          @prefix_key = ''
        end
      elsif keyval == 0xff1b # GDK_KEY_Escape
        @prefix_key = 'M-'
        send_key = false
      end
      $stderr.puts key_str if $DEBUG
      @frame.modeline(self)
      send_key
    end

    def select_font
      font = @frame.select_font
      if font != nil
        @frame.set_font(font[0], font[1])
        @current_buffer.mode.apply_theme(@frame.view_win, @theme)
      end
    end
  end
end
