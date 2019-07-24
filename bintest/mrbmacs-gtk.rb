$script_dir = File.dirname(__FILE__) + "/scripts/"
assert('init buffer') do
  o = `#{ENV['BUILD_BIN_PATH']}#{cmd('mrbmacs-gtk')} -l #{$script_dir}init_buffer`
  assert_equal 0, $?.exitstatus
  lines = o.split("\n")
  assert_equal "*scratch*", lines[0]
end