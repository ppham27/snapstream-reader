#!/bin/sh

if [ "$1" != "" ]; then
    BRANCH=$1
else
    BRANCH=develop
fi

# downloading

rm -rf snapstream-reader-$BRANCH
rm $BRANCH
wget https://github.com/ppham27/snapstream-reader/archive/$BRANCH.zip
unzip $BRANCH

# compiling

cd snapstream-reader-$BRANCH
mkdir build
cd build
cmake -DCMAKE_C_COMPILER=$HOME/usr/local/bin/gcc -DCMAKE_CXX_COMPILER=$HOME/usr/local/bin/g++ ..
make

# testing
ctest
./test/bin/date_unittest
./test/bin/snap_unittest
./test/bin/word_unittest
./test/bin/io_unittest
./test/bin/web_unittest
./test/bin/excerpt_unittest
./test/bin/expression_unittest
./test/bin/evaluator_unittest
./test/bin/distance_unittest
./test/bin/stringhasher_unittest
./test/bin/cooccurrencematrix_unittest

# installing
rm $HOME/html/test.html
rm $HOME/html/index.html
rm $HOME/html/hot-list.html
rm $HOME/html/style.css
rm $HOME/html/visualize.html
rm $HOME/html/d3.tip.js
rm $HOME/html/d3.v3.min.js
rm $HOME/html/graph.js
rm $HOME/html/multiple_time_varied.json
rm $HOME/html/cgi-bin/simple_search
rm $HOME/html/cgi-bin/near_search
rm $HOME/html/cgi-bin/multiple_search
rm $HOME/html/cgi-bin/pair_search
rm $HOME/html/cgi-bin/hot_list
rm $HOME/html/cgi-bin/process_file
cp index.html $HOME/html/test.html
cp index.html $HOME/html/index.html
cp hot-list.html $HOME/html/hot-list.html
cp style.css $HOME/html/style.css
cp visualize.html $HOME/html/visualize.html
cp d3.tip.js $HOME/html/d3.tip.js
cp d3.v3.min.js $HOME/html/d3.v3.min.js
cp graph.js $HOME/html/graph.js
cp multiple_time_varied.json $HOME/html/multiple_time_varied.json
cp cgi-bin/simple_search $HOME/html/cgi-bin
cp cgi-bin/near_search $HOME/html/cgi-bin
cp cgi-bin/multiple_search $HOME/html/cgi-bin
cp cgi-bin/pair_search $HOME/html/cgi-bin
cp cgi-bin/hot_list $HOME/html/cgi-bin
cp cgi-bin/process_file $HOME/html/cgi-bin/process_file
cp cgi-bin/dictionary.csv $HOME/html/cgi-bin/dictionary.csv

rm $HOME/scripts/check_snap.py
rm $HOME/scripts/top_weekly_countries.js
rm $HOME/scripts/hot_list.js
rm $HOME/resources/dictionary.csv
cp ../jobs/check_snap.py $HOME/scripts/
cp ../jobs/top_weekly_countries.js $HOME/scripts/
cp ../jobs/hot_list.js $HOME/scripts/
cp ../resources/dictionary.csv $HOME/resources/
