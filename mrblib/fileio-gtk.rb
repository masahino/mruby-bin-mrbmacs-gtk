module Mrbmacs
  class Application
    def read_file_name(prompt, directory)
      @frame.select_file(prompt, directory)
    end

  end
end