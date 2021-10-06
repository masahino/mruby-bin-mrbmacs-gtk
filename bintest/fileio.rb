$script_dir = File.dirname(__FILE__) + "/scripts/"
assert('find-file') do
  o = `#{cmd('mrbmacs-gtk')} -l #{$script_dir}find_file`
  assert_equal 0, $?.exitstatus
  lines = o.split("\n")
  assert_equal "test.input", lines[0]
  assert_equal "test.input", lines[1]
end
