module Mrbmacs
  class Application
    include Scintilla

    def add_buffer_to_frame(buffer)
      @frame.add_new_tab(buffer)
      @keymap.set_keymap(@frame.view_win)
      @frame.set_default_style
      @frame.edit_win.set_default_style(@theme)
      @frame.set_style_gtk
      buffer.mode.set_style(@frame.view_win, @theme)
    end

    def sci_notify(n)
      win = @frame.view_win

      if @sci_handler[n['code']] != nil
        @sci_handler[n['code']].call(self, n)
      end
    end

    def key_press(state, keyval)
      send_key = true
      mod_str = ""
      if (state & (1<<2)) == (1<<2) # CONTROL_MASK
        mod_str = "C-"
      end
      if (state & (1<<28)) == (1<<28) # META_MASK
        mod_str = "M-"
      end
      input_str = ""
      if keyval < 256
        input_str = keyval.chr
        if keyval == 0x020 # GDK_KEY_Space
          if mod_str == "C-"
            input_str = "@"
          end
        end
      elsif keyval == 0xff09 # GDK_KEY_Tab
        input_str = "Tab"
      elsif keyval == 0xff0d # GDK_KEY_Return
        input_str = "Enter"
      end

      key_str = @prefix_key + mod_str
#      if keyval < 256
      if input_str != ""
        key_str = key_str + input_str
        command = nil
        if @command_list.has_key?(key_str)
          command = @command_list[key_str]
        end
        if command != nil
          if command == "prefix"
            @prefix_key = key_str + " "
          else
            extend(command)
            @prefix_key = ""
          end
          send_key = false
        else
          @prefix_key = ""
        end
      elsif keyval == 0xff1b # GDK_KEY_Escape
        @prefix_key = "M-"
        send_key = false
      end
      @frame.modeline(self)
      return send_key
    end

  end
end
