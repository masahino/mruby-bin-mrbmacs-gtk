module Mrbmacs
  class Application
    def replace_string(str, newstr, start_pos, end_pos, query = false)
      @frame.view_win.sci_begin_undo_action
      @frame.view_win.sci_set_target_start(start_pos)
      @frame.view_win.sci_set_target_end(end_pos)
      while (pos = @frame.view_win.sci_search_in_target(str.length, str)) != -1
        if query == true
          @frame.view_win.sci_set_sel(@frame.view_win.sci_get_target_start,
            @frame.view_win.sci_get_target_end)
          case @frame.y_or_n("Query replacing #{str} with #{newstr}:")
          when true
            @frame.view_win.sci_replace_target(newstr.length, newstr)
          when nil # cancel
            @frame.echo_puts("Quit")
            break
          end
        else
          @frame.view_win.sci_replace_target(newstr.length, newstr)
        end
        @frame.view_win.sci_set_target_start(@frame.view_win.sci_get_target_end)
        @frame.view_win.sci_set_target_end(end_pos)
      end
      @frame.view_win.sci_end_undo_action
    end

    def query_replace
      @frame.query_replace
    end

    def replace_forward
      str = @frame.search_entry_get_text
      newstr = @frame.replace_entry_get_text
      replace_string(str, newstr, @frame.view_win.sci_get_anchor,
        @frame.view_win.sci_get_length, true)
    end

    def replace_backward
      str = @frame.search_entry_get_text
      newstr = @frame.replace_entry_get_text
      replace_string(str, newstr, @frame.view_win.sci_get_current_pos,
        0, true)
    end
  end
end