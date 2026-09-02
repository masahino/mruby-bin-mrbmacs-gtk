module Mrbmacs
  # GTK implementations of commands that need echo-area interaction.
  module Command
    def replace_string
      start_replace(false)
    end

    def query_replace
      start_replace(true)
    end
  end
end
