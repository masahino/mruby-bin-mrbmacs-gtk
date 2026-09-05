MRuby::Gem::Specification.new('mruby-bin-mrbmacs-gtk') do |spec|
  spec.license = 'MIT'
  spec.author  = 'masahino'
  spec.version = '0.4.0'

  version_text = File.join(spec.build_dir, 'version.txt')
  generated_version = File.join(spec.build_dir, 'generated_version.rb')

  file version_text => __FILE__ do
    FileUtils.mkdir_p(spec.build_dir)
    File.open(version_text, 'w') { |file| file.puts spec.version }
  end

  file generated_version => version_text do
    version = File.read(version_text).strip
    File.open(generated_version, 'w') do |file|
      file.puts 'module Mrbmacs'
      file.puts '  class Application'
      file.puts "    Version = #{version.inspect}"
      file.puts '  end'
      file.puts 'end'
    end
  end

  spec.rbfiles << generated_version

  spec.add_dependency 'mruby-mrbmacs-base', github: 'masahino/mruby-mrbmacs-base'
  spec.add_dependency 'mruby-scintilla-gtk', github: 'masahino/mruby-scintilla-gtk'
  spec.bins = %w[mrbmacs-gtk]

  spec.cc.flags << `pkg-config --cflags gtk+-3.0`.chomp
  if RUBY_PLATFORM.include?('darwin')
    spec.mruby.cc.flags << `pkg-config --cflags gtk-mac-integration-gtk3`.chomp
  end

  if RUBY_PLATFORM.include?('darwin')
    spec.mruby.linker.flags_before_libraries << `pkg-config --libs gtk-mac-integration-gtk3`.chomp
  end
end
