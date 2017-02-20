module Mrbmacs
  class Application
    include Scintilla

    def sci_notify(n, code)
      win = @frame.view_win
      if code == Scintilla::SCN_CHARADDED
        if win.sci_autoc_active == 0
          len, candidates = @current_buffer.mode.get_completion_list(win)
          if len > 0
            win.sci_autoc_show(len, candidates)
          end
        end
        pos1 = win.sci_brace_match(win.sci_get_current_pos() -1, 0)
        if pos1 != -1
          pos = win.sci_get_current_pos() - 1
          col = win.sci_get_column(pos)
          c = win.sci_get_line(win.sci_line_from_position(pos)).chomp[col]
          if c == ')' or c == ']' or c == '}' or c == '>'
            win.sci_brace_highlight(pos1, pos)
          end
        end
      end
      if code == Scintilla::SCN_UPDATEUI
        pos1 = win.sci_brace_match(win.sci_get_current_pos(), 0)
        if pos1 != -1
          win.sci_brace_highlight(pos1, win.sci_get_current_pos())
        else
          win.sci_brace_highlight(-1, -1)
        end
      end
    end

    def key_press(state, keyval)
      send_key = true
      $stderr.puts "state = [#{state}], keyval = [#{keyval}]"
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

      $stderr.puts "prefix_key = #{@prefix_key}"
      key_str = @prefix_key + mod_str
#      if keyval < 256
      if input_str != ""
        key_str = key_str + input_str
        $stderr.puts "key_str = #{key_str}"
        command = nil
        if @command_list.has_key?(key_str)
          $stderr.puts @command_list[key_str]
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

    def editloopx()
      @prefix_key = ""
      @frame.mainwin.signal_connect("key-press-event") do |*o|
        e = o[0].get_struct
        key_str, command = doscan(@prefix_key, e)
        if command != nil
          if command == "prefix"
            @prefix_key = key_str + " "
          else
            extend(command)
            @prefix_key = ""
          end
        else
          @prefix_key = ""
        end

        @frame.modeline(self)
      end
#      Gtk::Widget.signal_connect(@frame.view_win.editor, "sci-notify") do |*o|
#        $stderr.puts "notify"
#      end
      Gtk::main
    end
  end
end
