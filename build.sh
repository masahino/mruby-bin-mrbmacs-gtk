export MRUBY_CONFIG=`pwd`/misc/build_config.rb

if [ ! -d "./mruby/src" ]; then
  git clone -b 1.3.0 https://github.com/mruby/mruby.git
fi
(cd mruby; ./minirake)
(cd mruby; ./minirake test)
