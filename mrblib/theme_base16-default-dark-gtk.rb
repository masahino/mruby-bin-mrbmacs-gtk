module Mrbmacs
  class Base16DefaultDarkTheme < Base16Theme
    def initialize
      @@base00 = 0x181818
      @@base01 = 0x282828
      @@base02 = 0x383838
      @@base03 = 0x585858
      @@base04 = 0xb8b8b8
      @@base05 = 0xd8d8d8
      @@base06 = 0xe8e8e8
      @@base07 = 0xf8f8f8
      @@base08 = 0xab4642
      @@base09 = 0xdc9656
      @@base0A = 0xf7ca88
      @@base0B = 0xa1b56c
      @@base0C = 0x86c1b9
      @@base0D = 0x7cafc2
      @@base0E = 0xba8baf
      @@base0F = 0xa16946
      super
      @name = "base16-default-dark"
    end
  end  
end
