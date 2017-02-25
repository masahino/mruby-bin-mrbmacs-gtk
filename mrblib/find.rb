module Mrbmacs
  class Application
    def isearch_forward()
      win = @frame.view_win
      $stderr.puts "isearch_forward"
      win.sci_set_target_start(win.sci_get_current_pos)
      win.sci_set_target_end(win.sci_get_length)
      search_text = @frame.search_entry_get_text
      ret = win.sci_search_in_target(search_text.length, search_text)
      if ret != -1
        win.sci_set_sel(win.sci_get_target_start, win.sci_get_target_end)
      end
    end

  end
end