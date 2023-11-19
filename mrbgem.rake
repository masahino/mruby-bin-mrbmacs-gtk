MRuby::Gem::Specification.new('mruby-bin-mrbmacs-gtk') do |spec|
  spec.license = 'MIT'
  spec.author  = 'masahino'
  spec.version = '0.4.0'
  spec.add_dependency 'mruby-scintilla-gtk', github: 'masahino/mruby-scintilla-gtk'
  spec.add_dependency 'mruby-mrbmacs-base', github: 'masahino/mruby-mrbmacs-base'
  spec.add_test_dependency 'mruby-require'
  spec.bins = %w[mrbmacs-gtk]
end
