module Mrbmacs
  class ApplicationGtk < Application
    def read_file_name(prompt, directory)
      @frame.select_file(prompt, directory)
    end

    def find_file(filename = nil)
      if filename == nil
        dir = @current_buffer.directory
        filename = read_file_name("find file: ", dir)
        @frame.modeline_refresh(self)
      end
      if filename != nil
        if Mrbmacs::get_buffer_from_path(@buffer_list, filename) != nil
          switch_to_buffer(Mrbmacs::get_buffer_from_path(@buffer_list, filename).name)
        else
          @current_buffer.pos = @frame.view_win.sci_get_current_pos
          new_buffer = Buffer.new(filename)
#          if @current_buffer.docpointer != nil
#            @frame.view_win.sci_add_refdocument(@current_buffer.docpointer)
#          end
#          @frame.view_win.sci_set_docpointer(nil)
          add_new_buffer(new_buffer)
          @current_buffer = new_buffer
          add_buffer_to_frame(@current_buffer)
          open_file(filename)
          new_buffer.docpointer = @frame.view_win.sci_get_docpointer
          @frame.set_theme(@theme)
          @keymap.set_keymap(@frame.view_win)
          @frame.view_win.sci_set_lexer_language(@current_buffer.mode.lexer)
          @current_buffer.mode.set_style(@frame.view_win, @theme)
          @frame.view_win.sci_set_sel_back(true, 0xff0000)
          @frame.set_buffer_name(@current_buffer.name)
          @frame.edit_win.buffer = @current_buffer
          @frame.modeline(self)
          error = @current_buffer.mode.syntax_check(@frame.view_win)
          if error.size > 0
            @frame.show_annotation(error[0], error[1], error[2])
          end
        end
        after_find_file(self, filename)
      end
    end
  end
end