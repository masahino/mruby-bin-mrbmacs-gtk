module Mrbmacs
  class ApplicationGtk < ApplicationGui
    # Build a key string and dispatch it while echo owns the keyboard.
    # Returns true when mrbmacs consumed the key, false to let the character
    # reach @echo_win (i.e. type it into the pattern).
    def echo_key(state, keyval)
      key = case keyval
            when 0xff1b then 'Escape'
            when 0xff08, 0xffff then 'DEL'
            when 0xff0d, 0xff8d then 'Enter'
            when 0xff09 then 'Tab'
            when 0x20..0x7e then "#{get_modifier_string(state)}#{keyval.chr}"
            end
      return false if key.nil?

      echo_key_press(key)
    end
  end
end
