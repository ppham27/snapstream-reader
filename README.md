snap
====

This is a program to search through Snapstream closed-caption files. For a sample implementation, see this link, [http://johnny.sas.upenn.edu/~pemantle/test.html](http://johnny.sas.upenn.edu/~pemantle/test.html).

Various parts of the application:
- [Searching](http://johnny.sas.upenn.edu/~pemantle/)
- [Visualizing](http://johnny.sas.upenn.edu/~pemantle/visualize.html?filename=tmp%2Fdaily_country%2F2015-10-18_visualization_1985169007.json&title=2015-10-18%2025%20%28%7Bunited%20kingdom%7D%20%2B%20%7Bscot#spring-embed)
- [Hot Lists](http://johnny.sas.upenn.edu/~pemantle/hot-list.html)

## Developer Guide

All the C++ code is in the `src` folder. All the JavaScript and HTML is in the `html` folder. 
The `jobs` folder contains some optional utility scripts to automate searches.


### Building

To compile and build, do the following
1. Make sure you're in the `snapstream-reader` directory
2. `mkdir build && cd build` to make the build somewhere else in case something goes wrong
3. `cmake ..` to build compilation script
4. `make` to compile
5. `ctest` to run unit tests

### Deploying

See `jobs/get_snap_branch.sh` for a sample deployment on a Apache server.

On your server, you'll want a directory structure like this
```
html-root (the root of where you want to serve the application out of, can be named whatever)
| cgi-bin 
| tmp
```

After building there are several statically linked binaries in `build/cgi-bin`. Copy those binaries to `html-root/cgi-bin` on Apache. 
In the `build` folder, there are several HTML, CSS, JavaScript, and JSON files. Copy them all to the `html-root`.

Now there will be a `dictionary.csv` file in `build/cgi-bin`. Copy this to your `html-root/cgi-bin`, too. The Snapstream closed-caption files need to go in `html-root/cgi-bin/Data` to be read by the binaries.

Finally, ensure that the `html-root/tmp` folder is writable and `html-root/cgi-bin` is readable. 
You can run `chmod -R 777 html-root/tmp` to do this.





