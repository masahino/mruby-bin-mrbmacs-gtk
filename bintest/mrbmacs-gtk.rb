$script_dir = File.dirname(__FILE__) + "/scripts/"

assert('report the generated frontend version') do
  version_file = File.join(
    ENV.fetch('BUILD_DIR'), 'mrbgems', GEMNAME, 'version.txt'
  )
  expected_version = File.read(version_file).strip
  output = `#{cmd('mrbmacs-gtk')} --version`

  assert_equal 0, $?.exitstatus
  assert_equal expected_version, output.strip
end

assert('init buffer') do
  o = `#{cmd('mrbmacs-gtk')} -l #{$script_dir}init_buffer`
  assert_equal 0, $?.exitstatus
  lines = o.split("\n")
  assert_equal "*scratch*", lines[0]
end
