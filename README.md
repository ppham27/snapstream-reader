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

See [jobs/get_snap_branch.sh](https://github.com/ppham27/snapstream-reader/blob/master/jobs/get_snap_branch.sh) for a sample deployment on a Apache server, which is run with
`sh get_snap_branch.sh <branch name>`, for example, `sh get_snap_branch.sh master`.

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

### Jobs

There are several daily and weekly jobs in the [jobs](https://github.com/ppham27/snapstream-reader/tree/master/jobs) folder. Currently, the jobs have hard-coded dependencies on their running environment, which is currently `johnny.sas.upenn.edu/~pemantle`. Files from this folder are in the `scripts` folder on `johnny`. They require `~/resources/dictionary.csv` to be present.

- [check_snap.py](https://github.com/ppham27/snapstream-reader/blob/master/jobs/check_snap.py): This Python script checks `~/snap/Data` for any missing data files. It will send an email to the hard-coded list in the script.
- [top_weekly_countries.js](https://github.com/ppham27/snapstream-reader/blob/master/jobs/top_weekly_countries.js): This node.js script performs a matrix search on the countries in [dictionary.csv](https://github.com/ppham27/snapstream-reader/blob/master/resources/dictionary.csv). It will send an email when finished. A JSON file will be deposited in `~/html/tmp/daily_country/` for data visualization.
- [hot_list.js](https://github.com/ppham27/snapstream-reader/blob/develop/jobs/hot_list.js): Runs a hot list job and emails the results.
- [republican_candidates.js](https://github.com/ppham27/snapstream-reader/blob/develop/jobs/republican_candidates.js): Runs a multiple search job with 2016 presidential candidates according to [dictionary.csv](https://github.com/ppham27/snapstream-reader/blob/master/resources/dictionary.csv). A file is deposited in `~/html/tmp/daily_2016_republicans/` for visualization purposes. 

All these jobs are currently set up to run as a cronjob on `johnny`. Run `crontab -e` to edit the times. 

### Front End
In the `html` folder, you will find find various front ends. `index.html` allows you to perform a Simple Search, Multiple Search, and Matrix Search. `graph.html` visualizes the results of a Matrix Search, and `time-series.html` visualizes the results of a multiple search.
