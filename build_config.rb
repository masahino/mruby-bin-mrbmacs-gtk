class MRuby::Build
  # mruby 3.3+ splits core objects into libmruby_core.a, but gem binaries
  # in this project still expect both archives to be linked.
  def libraries
    [libmruby_static, libmruby_core_static]
  end
end

MRuby::Build.new do |conf|
  if ENV['VisualStudioVersion'] || ENV['VSINSTALLDIR']
    conf.toolchain :visualcpp
  else
    conf.toolchain :gcc
  end

  conf.enable_debug

  # UTF-8-aware String (String#length / [] / index operate on characters).
  conf.cc.defines += %w[MRB_UTF8_STRING]

  conf.gembox 'default'

  conf.gem github: 'masahino/mruby-mrbmacs-lsp'
  conf.gem github: 'masahino/mruby-lsp-client' do |g|
    g.skip_test = true
  end
  conf.gem github: 'masahino/mruby-mrbmacs-dap'
  conf.gem github: 'masahino/mruby-mrbmacs-aichat'
  conf.gem github: 'masahino/mruby-mrbmacs-agent'
  conf.gem github: 'masahino/mruby-debug'
  conf.gem github: 'masahino/mruby-mrbmacs-themes-base16'
  conf.gem github: 'masahino/mruby-mrbmacs-themes-tomorrow'

  conf.gem github: 'mattn/mruby-iconv' do |g|
    g.linker.libraries.delete 'iconv' if RUBY_PLATFORM.include?('linux')
    g.skip_test = true
  end
  conf.gem github: 'iij/mruby-regexp-pcre' do |g|
    g.skip_test = true
  end

  conf.gem github: 'masahino/mruby-scintilla-gtk'

  conf.gem File.expand_path(__dir__)

  conf.enable_bintest
  conf.enable_test
end
