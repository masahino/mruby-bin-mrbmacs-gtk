export MRUBY_CONFIG=`pwd`/misc/build_config.rb
SCINTILLA_FILE='scintilla372.tgz'

if [ ! -f "./scintilla/bin/scintilla.a" ]; then
  wget http://www.scintilla.org/$SCINTILLA_FILE
  tar zxf $SCINTILLA_FILE
  (cd scintilla/gtk ; make GTK3=1)
fi
if [ ! -d "./mruby/src" ]; then
  git clone https://github.com/mruby/mruby.git
fi
(cd mruby; ./minirake)
(cd mruby; ./minirake test)
