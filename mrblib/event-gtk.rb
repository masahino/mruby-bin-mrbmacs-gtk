module Mrbmacs
  class ApplicationGtk
    def add_io_read_event(io, &proc)
      @readings.push io
      @io_handler[io] = proc
      add_gtk_io_callback(io)
    end

    def del_io_read_event(io)
      @readings.delete io
      @io_handler.delete(io)
      del_gtk_io_callback(io)
    end
  end
end