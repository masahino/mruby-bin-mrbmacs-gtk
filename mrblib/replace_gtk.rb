module Mrbmacs
  # Query-replace in the echo area, mirroring the Cocoa frontend
  # (replace_cocoa.rb). Search / replacement strings are read with echo_gets;
  # the per-match y / n / ! / q / C-g keys arrive through echo_key_press while
  # the frame is in echo_key_mode. To be shared with Cocoa once both are proven.
  class ApplicationGtk < Application
    def start_replace(query, search_text = nil, replacement_text = nil)
      if @frame.view_win.sci_get_readonly
        @frame.echo_puts('Buffer is read-only')
        return
      end

      search_text = @frame.echo_gets('Replace string: ', '') if search_text.nil?
      return if search_text.nil?

      if search_text.empty?
        @frame.echo_puts('Empty search string')
        return
      end
      if replacement_text.nil?
        replacement_text = @frame.echo_gets("Replace string #{search_text} with: ", '')
      end
      return if replacement_text.nil?

      if query
        begin_query_replace(search_text, replacement_text)
      else
        count = replace_all_from(@frame.view_win.sci_get_current_pos, search_text, replacement_text)
        @frame.echo_puts(replace_summary(count))
        @frame.modeline(self)
      end
    end

    def begin_query_replace(search_text, replacement_text)
      @query_replace_active = true
      @replace_search_text = search_text
      @replacement_text = replacement_text
      @replace_count = 0
      @replace_next_pos = @frame.view_win.sci_get_current_pos
      find_next_replace_match
    end

    def query_replace_active?
      @query_replace_active
    end

    def query_replace_key_press(key)
      case key
      when 'y', ' '
        replace_query_match
        find_next_replace_match if @query_replace_active
      when 'n', 'DEL'
        @replace_next_pos = @frame.view_win.sci_get_target_end
        find_next_replace_match
      when '!'
        replace_query_match
        if @query_replace_active
          @replace_count += replace_all_from(@replace_next_pos, @replace_search_text, @replacement_text)
          finish_query_replace_with_summary
        end
      when 'q', 'Enter'
        finish_query_replace_with_summary
      when 'C-g'
        finish_query_replace('Quit')
      end
      true
    end

    def find_next_replace_match
      view = @frame.view_win
      view.sci_set_target_start(@replace_next_pos)
      view.sci_set_target_end(view.sci_get_length)
      found = view.sci_search_in_target(@replace_search_text.bytesize, @replace_search_text)
      if found == -1
        finish_query_replace_with_summary
        return
      end

      view.sci_set_sel(view.sci_get_target_start, view.sci_get_target_end)
      search_highlight_begin(@replace_search_text, :replace)
      prompt = "Query replacing #{@replace_search_text} with #{@replacement_text}: (y, n, !, q) "
      @frame.start_query_replace(prompt)
      @frame.modeline(self)
    end

    def replace_query_match
      view = @frame.view_win
      with_undo_action do
        view.sci_replace_target(@replacement_text.bytesize, @replacement_text)
      end
      @replace_count += 1
      @replace_next_pos = view.sci_get_target_end
    end

    def replace_all_from(start_pos, search_text, replacement_text)
      view = @frame.view_win
      count = 0
      with_undo_action do
        next_pos = start_pos
        loop do
          view.sci_set_target_start(next_pos)
          view.sci_set_target_end(view.sci_get_length)
          break if view.sci_search_in_target(search_text.bytesize, search_text) == -1

          view.sci_replace_target(replacement_text.bytesize, replacement_text)
          count += 1
          next_pos = view.sci_get_target_end
        end
      end
      count
    end

    def with_undo_action
      @frame.view_win.sci_begin_undo_action
      yield
    ensure
      @frame.view_win.sci_end_undo_action
    end

    def finish_query_replace_with_summary
      finish_query_replace(replace_summary(@replace_count))
    end

    def finish_query_replace(message)
      search_highlight_end
      @query_replace_active = false
      @frame.finish_query_replace
      @frame.echo_puts(message)
      @frame.modeline(self)
    end

    def replace_summary(count)
      "Replaced #{count} occurrence#{count == 1 ? '' : 's'}"
    end
  end
end
