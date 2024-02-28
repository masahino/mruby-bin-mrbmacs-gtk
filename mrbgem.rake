MRuby::Gem::Specification.new('mruby-bin-mrbmacs-gtk') do |spec|
  spec.license = 'MIT'
  spec.author  = 'masahino'
  spec.version = '0.4.0'
  spec.add_dependency 'mruby-mrbmacs-base', github: 'masahino/mruby-mrbmacs-base'
  spec.add_dependency 'mruby-scintilla-gtk', github: 'masahino/mruby-scintilla-gtk'
  spec.add_test_dependency 'mruby-require'
  spec.bins = %w[mrbmacs-gtk]

  spec.cc.flags << `pkg-config --cflags gtk+-3.0`.chomp
  if RUBY_PLATFORM.include?('darwin')
    spec.mruby.cc.flags << `pkg-config --cflags gtk-mac-integration-gtk3`.chomp
  end
#
  if RUBY_PLATFORM.include?('darwin')
    spec.mruby.linker.flags_before_libraries << `pkg-config --libs gtk-mac-integration-gtk3`.chomp
  end
end
