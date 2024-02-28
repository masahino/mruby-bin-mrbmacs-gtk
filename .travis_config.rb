MRuby::Build.new do |conf|
  # load specific toolchain settings

  # Gets set by the VS command prompts.
  if ENV['VisualStudioVersion'] || ENV['VSINSTALLDIR']
    toolchain :visualcpp
  else
    toolchain :gcc
  end

  enable_debug

#  conf.cc.defines = %w(MRB_ENABLE_ALL_SYMBOLS)
  conf.cc.defines = %w(MRB_UTF8_STRING)

  conf.gembox 'default'
  conf.gem :github => 'masahino/mruby-mrbmacs-lsp'
  #conf.gem :github => 'masahino/mruby-mrbmacs-themes-base16', :branch => 'main'
  conf.gem :github => 'mattn/mruby-iconv' do |g|
    if RUBY_PLATFORM.include?('linux')
      g.linker.libraries.delete 'iconv'
    end
  end

  conf.gem :github => 'masahino/mruby-scintilla-gtk' do |g|
    g.download_scintilla
  end

  conf.gem "#{MRUBY_ROOT}/.."

  # for debugging
  #  conf.gem :github => 'masahino/mruby-debug', :branch => 'main'
  # bintest
  conf.enable_bintest
  conf.enable_test
end
