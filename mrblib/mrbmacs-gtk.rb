module Mrbmacs
  class Application
    include Scintilla

    def key_press(state, keyval)
      $stderr.puts "state = [#{state}], keyval = [#{keyval}]"
      mod_str = ""
      if (state & (1<<2)) == (1<<2) # CONTROL_MASK
        mod_str = "C-"
      end
      if (state & (1<<28)) == (1<<28) # META_MASK
        mod_str = "M-"
      end

      $stderr.puts "prefix_key = #{@prefix_key}"
      key_str = @prefix_key + mod_str
      if keyval < 256
        key_str = key_str + keyval.chr
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
        else
          @prefix_key = ""
        end
      elsif keyval == 0xff1b # GDK_KEY_Escape
        @prefix_key = "M-"
      end
      @frame.modeline(self)
    end

    def editloopx()
      $stderr.puts "editloop"
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
