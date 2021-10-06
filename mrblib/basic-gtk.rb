module Mrbmacs
  class ApplicationGtk < Application
    def keyboard_quit
      super
      if @frame.view_win.sci_get_focus == false
        @frame.stop_search
        @frame.stop_replace
        @frame.view_win.sci_grab_focus
      end
    end
  end
end
