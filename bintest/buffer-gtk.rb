$script_dir = File.dirname(__FILE__) + "/scripts/"
assert('update_buffer_window') do
  o = `#{cmd('mrbmacs-gtk')} -l #{$script_dir}update_buffer_window`
  assert_equal 0, $?.exitstatus
  lines = o.split("\n")
  assert_equal "*Messages*", lines[0], "@current_buffer.name"
  assert_equal "*Messages*", lines[1], "@frame.edit_win.buffer.name"
  assert_equal "test.input", lines[2], "@current_buffer.name"
  assert_equal "test.input", lines[3], "@frame.edit_win.buffer.name"
end
