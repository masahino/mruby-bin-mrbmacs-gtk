module Mrbmacs
  class ApplicationGtk < Application
    #    def switch_to_buffer(buffername = nil)
    #      $stderr.puts "switch to buffer"
    #    end
    def update_buffer_window(new_buffer)
      @current_buffer.pos = @frame.view_win.sci_get_current_pos
      #      @frame.view_win.sci_add_refdocument(@current_buffer.docpointer)
      #      @frame.view_win.sci_set_docpointer(new_buffer.docpointer)
      @current_buffer = new_buffer
      edit_win = @frame.edit_win_list.select { |w| w.buffer == @current_buffer }[0]
      @frame.edit_win = edit_win
      @frame.view_win = edit_win.sci
      @buffer_list.push(@buffer_list.delete(new_buffer))
      #     @frame.view_win.sci_set_lexer_language(@current_buffer.mode.name)
      apply_theme_to_mode(@current_buffer.mode, @frame.edit_win, @theme)
      #      @frame.view_win.sci_goto_pos(@current_buffer.pos)
      @frame.sync_tab(@current_buffer.name)
      @frame.modeline(self)
    end
  end
end
