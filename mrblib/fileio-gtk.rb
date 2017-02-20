module Mrbmacs
  class Application
    def find_file(filename = nil)
      filename = @frame.select_file("find file:", @current_buffer.directory)
      if filename != nil
        view_win = @frame.view_win
        @current_buffer.pos = view_win.sci_get_current_pos
        new_buffer = Buffer.new(filename, @buffer_list.map{|b| b.name})
        view_win.sci_add_refdocument(@current_buffer.docpointer)
        view_win.sci_set_docpointer(nil)
        new_buffer.docpointer = view_win.sci_get_docpointer
        @buffer_list.push(new_buffer)
        @prev_buffer = @current_buffer
        @current_buffer = new_buffer
        load_file(filename)
        view_win.sci_set_lexer_language(@current_buffer.mode.name)
        @current_buffer.mode.set_style(view_win, @theme)
        view_win.sci_set_sel_back(true, 0xff0000)
      end
    end
  end
end