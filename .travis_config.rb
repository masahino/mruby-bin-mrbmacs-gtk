MRuby::Build.new do |conf|
  # load specific toolchain settings

  # Gets set by the VS command prompts.
  if ENV['VisualStudioVersion'] || ENV['VSINSTALLDIR']
    toolchain :visualcpp
  else
    toolchain :gcc
  end

  enable_debug

  conf.cc.defines = %w(MRB_ENABLE_ALL_SYMBOLS)
  conf.cc.defines = %w(MRB_UTF8_STRING)

  # Use mrbgems
  # conf.gem 'examples/mrbgems/ruby_extension_example'
  # conf.gem 'examples/mrbgems/c_extension_example' do |g|
  #   g.cc.flags << '-g' # append cflags in this gem
  # end
  # conf.gem 'examples/mrbgems/c_and_ruby_extension_example'
  # conf.gem :github => 'masuidrive/mrbgems-example', :checksum_hash => '76518e8aecd131d047378448ac8055fa29d974a9'
  # conf.gem :git => 'git@github.com:masuidrive/mrbgems-example.git', :branch => 'master', :options => '-v'
  conf.gembox 'default'
  conf.gem :github => 'masahino/mruby-mrbmacs-lsp'
  conf.gem :github => 'masahino/mruby-mrbmacs-themes-base16', :branch => 'main'
  conf.gem "#{MRUBY_ROOT}/mrbgems/mruby-eval"
  conf.gem "#{MRUBY_ROOT}/mrbgems/mruby-exit"
  conf.gem "#{MRUBY_ROOT}/mrbgems/mruby-bin-mrbc"
  conf.gem :github => 'mattn/mruby-onig-regexp'
  conf.gem :github => 'fastly/mruby-optparse'
  conf.gem :github => 'mattn/mruby-iconv' do |g|
    if RUBY_PLATFORM.include?('linux')
      g.linker.libraries.delete 'iconv'
    end
  end

  conf.gem :github => 'masahino/mruby-scintilla-base', :branch => "scintilla5" do |g|
    g.cc.flags << `pkg-config --cflags gtk+-3.0`.chomp
    g.download_scintilla
  end
  conf.gem :github => 'masahino/mruby-scintilla-gtk', :branch => "scintilla5" do |g|
    g.cc.flags << `pkg-config --cflags gtk+-3.0`.chomp
    g.download_scintilla
    g.linker.flags_before_libraries << `pkg-config --libs gmodule-2.0 gtk+-3.0`.chomp
  end
  conf.gem :github => 'masahino/mruby-mrbmacs-base', :branch => "scintilla5"

  conf.gem "#{MRUBY_ROOT}/.."
  conf.linker.libraries << "stdc++"

   conf.cc do |cc|
    cc.flags << `pkg-config --cflags gtk+-3.0`.chomp
    if RUBY_PLATFORM.include?('darwin')
      cc.flags << `pkg-config --cflags gtk-mac-integration-gtk3`.chomp
    end
  #   cc.command = ENV['CC'] || 'gcc'
  #   cc.flags = [ENV['CFLAGS'] || %w()]
  #   cc.include_paths = ["#{root}/include"]
  #   cc.defines = %w(DISABLE_GEMS)
  #   cc.option_include_path = '-I%s'
  #   cc.option_define = '-D%s'
  #   cc.compile_options = "%{flags} -MMD -o %{outfile} -c %{infile}"
   end

  # mrbc settings
  # conf.mrbc do |mrbc|
  #   mrbc.compile_options = "-g -B%{funcname} -o-" # The -g option is required for line numbers
  # end

  # Linker settings
  conf.linker do |linker|
    if RUBY_PLATFORM.include?('darwin')
      linker.flags_before_libraries << `pkg-config --libs gtk-mac-integration-gtk3`.chomp
    end
  #   linker.command = ENV['LD'] || 'gcc'
  #   linker.flags = [ENV['LDFLAGS'] || []]
  #   linker.flags_before_libraries = []
  #   linker.libraries = %w()
  #   linker.flags_after_libraries = []
  #   linker.library_paths = []
  #   linker.option_library = '-l%s'
  #   linker.option_library_path = '-L%s'
  #   linker.link_options = "%{flags} -o %{outfile} %{objs} %{libs}"
  end

  # Archiver settings
  # conf.archiver do |archiver|
  #   archiver.command = ENV['AR'] || 'ar'
  #   archiver.archive_options = 'rs %{outfile} %{objs}'
  # end

  # Parser generator settings
  # conf.yacc do |yacc|
  #   yacc.command = ENV['YACC'] || 'bison'
  #   yacc.compile_options = '-o %{outfile} %{infile}'
  # end

  # gperf settings
  # conf.gperf do |gperf|
  #   gperf.command = 'gperf'
  #   gperf.compile_options = '-L ANSI-C -C -p -j1 -i 1 -g -o -t -N mrb_reserved_word -k"1,3,$" %{infile} > %{outfile}'
  # end

  # file extensions
  # conf.exts do |exts|
  #   exts.object = '.o'
  #   exts.executable = '' # '.exe' if Windows
  #   exts.library = '.a'
  # end

  # file separetor
  # conf.file_separator = '/'

  # bintest
  conf.enable_bintest
  conf.enable_test
end
