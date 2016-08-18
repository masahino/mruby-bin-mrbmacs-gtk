module Mrbmacs
  class Application
    include Scintilla

    def doscan(prefix, e)
      mod_str = ""
      case e[:state]
      when Gdk::ModifierType::CONTROL_MASK
        mod_str = "C-"
      when Gdk::ModifierType::META_MASK
        mod_str = "M-"
      end
      key_str = prefix + mod_str
      if e[:keyval] < 256
        key_str = key_str + e[:keyval].chr
      end
      if @command_list.has_key?(key_str)
        $stderr.puts @command_list[key_str]
        return [key_str, @command_list[key_str]]
      end
      [key_str, nil]
    end

    def editloop()
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
