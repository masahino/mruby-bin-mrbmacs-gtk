module Mrbmacs
  class Application
    def isearch()
      win = @frame.view_win
      search_text = @frame.search_entry_get_text
      win.sci_set_target_start(@target_start_pos)
      win.sci_set_target_end(@target_end_pos)
      ret = win.sci_search_in_target(search_text.length, search_text)
      if ret != -1
        win.sci_set_sel(win.sci_get_target_start, win.sci_get_target_end)
#        win.sci_set_target_end(win.sci_get_length)
      else
        $stderr.puts "not matched"
        @target_start_pos = 0
      end
    end

    def isearch_forward()
      win = @frame.view_win
      $stderr.puts "isearch_forward"
      $stderr.puts "["+win.sci_get_target_text+"]"
      @target_start_pos = win.sci_get_current_pos
      @target_end_pos = win.sci_get_length
      isearch()
    end

    def isearch_backward()
      win = @frame.view_win
      $stderr.puts "isearch_wackward"
      $stderr.puts "["+win.sci_get_target_text+"]"
      @target_start_pos = win.sci_get_current_pos - @frame.search_entry_get_text.length
      @target_end_pos = 0
      isearch()
    end
  end
end