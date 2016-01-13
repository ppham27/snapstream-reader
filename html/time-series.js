var variables = ["Total Matches", "Programs", "Contexts"];
var movingAverageWindow = 7;
var width = parseInt(window.getComputedStyle(document.getElementById('graph')).width);
var legendWidth = 160;
var height = 460;
var margin = {top: 10, right: 20, bottom: 20, left: 55};
var urlQuery = parseQueryString(window.location.search.slice(1));
var fileName = urlQuery['filename'] || 'default-time-series.csv';
var title = urlQuery['title'] || null;
if (document.getElementById('graph-title')) {
  if (title === null) {
    var graphTitle = document.getElementById('graph-title');
    graphTitle.parentNode.removeChild(graphTitle);
  } else {
    document.getElementById('graph-title').textContent = title;
  }
}
// graph
var controls = d3.select("#graph")
               .append("div").attr("id", "controls")
               .style("width", width + "px")
               .style("height", (500-height)/2 + "px");
var legend = d3.select("#graph") 
             .append("div")
             .attr("id", "legend")
             .style("width", legendWidth + "px")
             .style("height", height + "px")
             .style("display", "block")
             .style("float", "right");
var svg = d3.select("#graph")
          .append("svg")
          .attr("width", width - legendWidth)
          .attr("height", height)
          .style("display", "block");
var info = d3.select('#graph')
           .append("div").attr("id", "info")
           .style("width", width + "px")
           .style("height", (500-height)/2 + "px");
if (!isMobile()) {              //no zoom capabilities on mobile
  info.append("button").attr("type", "button")
  .text("Reset Zoom")
  .on("click", brushed);
}
info.append("span")
.html("Mouse over legend to highlight particular candidates. Click and hold <em>Control</em> key and mouse over graph to see data points.");
svg.append("clipPath")
.attr("id", "clip")
.append("rect")
.attr("x", margin.left).attr("y", margin.top-5)
.attr("width", width - legendWidth - margin.left - margin.right + 5)
.attr("height", height - margin.top - margin.bottom + 5);
legend.append("h2").text("Legend");
legend = legend.append("ul");

// set up axes
var x = d3.time.scale().range([margin.left, width - legendWidth - margin.right]);
var y = d3.scale.linear().range([height - margin.bottom , margin.top]);
var xAxis = d3.svg.axis()
            .scale(x).orient("bottom")
            .innerTickSize(-height + margin.bottom + margin.top)
            .outerTickSize(8).tickPadding(8)
            .ticks(d3.time.week, 1)
            .tickFormat(function(d) {
              if (x.ticks().length > 1 &&
                  (x.ticks()[1] - x.ticks()[0])/1000/60/60/24 >= 28) {
                return d3.time.format("%b '%y")(d);
              }
              return d3.time.format("%b %d")(d);
            });
var yAxis = d3.svg.axis()
            .innerTickSize(-(width - legendWidth) + margin.left + margin.right)
            .outerTickSize(8).tickPadding(8)
            .scale(y).orient("left")
            .tickFormat(d3.format("%"));
svg.append("g")
.attr("class", "x axis")
.attr("transform", "translate(0," + (height - margin.bottom) + ")");
// // x axis title
// d3.select(".x.axis")
// .append("text").attr("id", "x-axis-title")
// .attr("x", margin.left + (width - legendWidth - margin.right - margin.left)/2)
// .attr("y", 45).attr("text-anchor", "middle")
// .text("Date");
svg.append("g")
.attr("class", "y axis")
.attr("transform", "translate(" + margin.left + "," + 0 + ")");
var yAxisTitle = d3.select(".y.axis")
                 .append("text").attr("id", "y-axis-title")
                 .attr("x", -45)
                 .attr("y", margin.top + (height - margin.top - margin.left)/2)
                 .attr("text-anchor", "middle")
                 .attr("transform", "rotate(-90, -45," + (margin.top + (height - margin.top - margin.left)/2) + ")")
                 .text(variables[0] + " Moving Average");

// build controls
controls = controls.append("form");
var variableSelector = controls.append("fieldset");
variableSelector.append("label").text("Variable:")
.attr("for", "variable");
variables.forEach(function(d, idx) {
  var input = variableSelector.append("input")
              .attr("type", "radio")
              .attr("name", "variable")
              .attr("value", d)
              .on("change", update);
  if (idx === 0) input[0][0].checked = true;
  variableSelector.append("span").attr("class", "radio-label").text(d);
});
controls.append("div")
.style("display", "inline-block")
.style("width", (width - 590) + "px");
var isMovingAverage = controls.append("fieldset");
isMovingAverage.append("label").text("Moving Average:")
.attr("for", "isMovingAverage")
.attr("class", "toggle");
isMovingAverage.append("input")
.attr("type", "checkbox")
.attr("id", "isMovingAverage")
.attr("name", "isMovingAverage")
.on("change", update)
.node().checked = true;
var isPercentSelector = controls.append("fieldset");
isPercentSelector.append("label").text("Percentage:")
.attr("for", "isPercent")
.attr("class", "toggle");
isPercentSelector.append("input")
.attr("type", "checkbox")
.attr("id", "isPercent")
.attr("name", "isPercent")
.on("change", update)
.node().checked = true;

// set up paths
var liner = d3.svg.line()
            .x(function(d) { return x(d.Date); })
            .y(function(d) { return y(d._value); });
var strokeColor = d3.scale.category10();
// point tip
var pointTip = d3.tip()
               .attr('class', 'graph tip')
               .direction('e')
               .offset([0,5])
               .html(function (d) {
                 var floatFormatter = d3.format('.1f');
                 var percentFormatter = d3.format('%');                 
                 var html = d.Term + ', ' + d.day;
                 variables.forEach(function(dd) {
                   html += '<br><strong>'+dd+':</strong> <span style="color:#e41a1c">';
                   html += d[dd] + ' (' + percentFormatter(d[dd]/dailyTotals[d.day][dd]) + ')</span>';
                 });
                 if (d.Date >= movingAverageMinDate) {
                   variables.forEach(function(dd) {
                     var key = dd + " Moving Sum";
                     html += '<br><strong>'+dd+' Moving Average:</strong> <span style="color:#e41a1c">';
                     html += floatFormatter(d[key]/movingAverageWindow) + ' (' + percentFormatter(d[key]/dailyTotals[d.day][key]) + ')</span>';
                   });
                 }
                 return html;
               });

// set up zoom brush
var brush = d3.svg.brush()
            .x(x).y(y)
            .on("brushend", brushed);
if (!isMobile()) {
  window.addEventListener("keydown", function(e) {
    if (e.keyCode === 17) {
      var brushGroups = Array.prototype.slice.call(document.querySelectorAll('g.brush'));
      brushGroups.forEach(function(brushGroup) {
        if (brushGroup) brushGroup.parentNode.removeChild(brushGroup);
      });
    }
  });
  window.addEventListener("keyup", function(e) {
    if (e.keyCode === 17) svg.append("g").attr("class", "brush").call(brush);
  });
}

var data, minDate, maxDate, movingAverageMinDate;     //global data variables
var dailyTotals = {};
var initialized = false;
d3.csv(fileName, function(err, rawData) {
  if (err) {
    console.error(err);
  } else {
    initializeData(rawData);
  }
});


function initializeData(rawData) {
  initialized = true;
  rawData.forEach(function(d) {
    if (dailyTotals[d.Date] === undefined) {
      dailyTotals[d.Date] = {};
      variables.forEach(function(variable) { dailyTotals[d.Date][variable] = 0; });
    }
    d.day = d.Date;
    variables.forEach(function(variable) { 
      d[variable] = parseInt(d[variable]); 
      dailyTotals[d.Date][variable] += d[variable];
    });
  });
  data = d3.nest()
         .key(function(d) { return d.Term; })  
         .entries(rawData);
  data.forEach(function(d) {
    d.values.forEach(function(d) {
      d.Date = d3.time.format("%Y-%m-%d").parse(d.Date);
    })
    d.values.sort(function(a, b) { return a.Date - b.Date; });
  });
  data.sort(function(a, b) {
    var aSum = d3.sum(a.values.slice(-3), function(d) { return d[variables[0]]; });
    var bSum = d3.sum(b.values.slice(-3), function(d) { return d[variables[0]]; });
    return bSum - aSum;
  });
  data = data.slice(0, 9);
  strokeColor.domain(data.map(function(d) { return d.key; }));                     
  // set up legend
  legend.selectAll("li")
  .data(data, function(d) { return d.key; })
  .enter()
  .append("li")
  .each(function(d) {
    var w = 40;
    var h = 30;
    var svg = d3.select(this).append("svg")
              .attr("width", legendWidth)
              .attr("height", 30);
    svg.append("line")
    .attr("x1", 0).attr("y1", h/2)
    .attr("x2", w).attr("y2", h/2)
    .style("stroke", strokeColor(d.key));
    svg.append("text").text(d.key)
    .attr("x", w + 5).attr("y", h/2).attr("dy", "5px");
    svg.on("mouseover", function() {
      d3.selectAll("path.data")
      .filter(function(dd) {
        return dd.key === d.key;
      }).transition().duration(250)
      .style("stroke-width", 3);
    })
    .on("mouseout", function() {
      d3.selectAll("path.data")
      .filter(function(dd) {
        return dd.key === d.key;
      }).transition().duration(250)
      .style("stroke-width", 1);
    });
  });
  minDate = d3.min(data[0].values, function(d) { return d.Date; });
  maxDate = d3.max(data[0].values, function(d) { return d.Date; });
  if (Object.keys(dailyTotals).length < 2*movingAverageWindow) movingAverageWindow = 1;
  calculateMovingAverage(data, dailyTotals);
  movingAverageMinDate = d3.min(Object.keys(dailyTotals).filter(function(dt) { return dailyTotals[dt][variables[0] + " Moving Sum"] !== undefined; }));
  movingAverageMinDate = d3.time.format("%Y-%m-%d").parse(movingAverageMinDate);
  var valueRange = calculateValue(data, getVariable(), getIsMovingAverage(), getIsPercent());
  drawAxes(getIsMovingAverage() ? movingAverageMinDate : minDate, 
           maxDate, valueRange[0], valueRange[1], getIsPercent());
  var paths = svg.selectAll("path.data")
              .data(data, function(d) { return d.key; })
              .enter().append("path")
              .attr("clip-path", "url(#clip)")
              .attr("class", "data")         
              .style("fill", "none")
              .style("stroke", function(d) { return strokeColor(d.key); }); 
  svg.selectAll("circle.data")
  .data([].concat.apply([], data.map(function(d) { return d.values; })), 
        function(d) { return d.Date.toJSON() + "|" + d.Term; })
  .enter().append("circle")
  .attr("clip-path", "url(#clip)")
  .attr("class", "data")
  .attr("r", 5)
  .style("fill", function(d) { return strokeColor(d.Term); })
  .style("fill-opacity", 0)
  .style("stroke", "none")
  .on("mouseover", function(d) {
    pointTip.show(d, this);
    d3.select(this).transition().duration(250).style("fill-opacity", 1);
  })
  .on("mouseout", function(d) {
    pointTip.hide(d, this);
    d3.select(this).transition().duration(250).style("fill-opacity", 0);
  });
  draw();
  if (!isMobile()) svg.append("g").attr("class", "brush").call(brush);
  svg.call(pointTip);
}


function calculateMovingAverage(data, dailyTotals) {
  // assume data is sorted
  data.forEach(function(d) { 
    variables.forEach(function(variable) {
      var cumulativeSum = [0];
      d.values.forEach(function(dd, idx) {
        cumulativeSum.push(cumulativeSum[idx] + dd[variable]);        
        if (idx + 1 >= movingAverageWindow) {
          var newKey = variable + " Moving Sum";
          dd[newKey] = cumulativeSum[idx + 1] - cumulativeSum[idx + 1 - movingAverageWindow];
          if (dailyTotals[dd.day][newKey] === undefined) dailyTotals[dd.day][newKey] = 0;
          dailyTotals[dd.day][newKey] += dd[newKey];
        }
      });
    });
  });
}

function calculateValue(data, variable, isMovingAverage, isPercent) {
  var minValue = Number.MAX_VALUE;
  var maxValue = 0;
  data.forEach(function(d) {
    d.values.forEach(function(dd) {
      var key = isMovingAverage ? variable + " Moving Sum" : variable;
      var value = dd[key];
      if (isMovingAverage && !isPercent) {
        value /= movingAverageWindow;
      }  else if (isPercent) {
        value /= dailyTotals[dd.day][key];
      }
      if (!isNaN(value)) {
        if (minValue > value) minValue = value;
        if (maxValue < value) maxValue = value;
        dd._value = value;
      } else {
        dd._value = -1;
      }      
    });
  });
  return [minValue, maxValue];
}

function drawAxes(minDate, maxDate, minValue, maxValue, percent) {
  x.domain([minDate, maxDate]);
  y.domain([minValue, maxValue]);   
  var daysRange = Math.ceil((maxDate - minDate)/24/60/60/1000);
  var daysInterval = Math.ceil(daysRange/10);
  if (daysInterval <= 5) {    
    xAxis.ticks({range: timeDay}, daysInterval);
  } else if (daysInterval <= 10) {
    xAxis.ticks(d3.time.week, 1);
  } else if (daysInterval <= 17) {
    xAxis.ticks(d3.time.week, 2);
  } else if (daysInterval <= 40) {
    xAxis.ticks(d3.time.month, 1);
  } else {
    xAxis.ticks(10);
  }
  if (percent === true) {
    yAxis.tickFormat(d3.format("%"));
  } else {
    yAxis.tickFormat(d3.format("d"));
  }
  d3.select("g.x.axis").transition().duration(1000).call(xAxis);  
  d3.select("g.y.axis").transition().duration(1000).call(yAxis);  
}

function draw() {
  svg.selectAll("path.data")
  .transition().duration(1000)
  .attr("d", function(d) { return liner(d.values); })
  svg.selectAll("circle.data")
  .transition().duration(1000)
  .attr("cx", function(d) { return x(d.Date); })
  .attr("cy", function(d) { return y(d._value); });
}

function getVariable() {
  var inputs = Array.prototype.slice.call(document.querySelectorAll('input[name="variable"]'));
  return inputs.filter(function(input) { return input.checked; })[0].value;

}

function getIsPercent() {
  return document.getElementById("isPercent").checked;
}

function getIsMovingAverage() {
  return document.getElementById("isMovingAverage").checked;
}

function brushed() {
  if (brush.empty()) {
    update();
  } else {
    var variable = getVariable();
    var isPercent = getIsPercent();
    var isMovingAverage = getIsMovingAverage();
    yAxisTitle.text(isMovingAverage ? variable + " Moving Average" : variable);
    var valueRange = calculateValue(data, variable, isMovingAverage, isPercent);
    var extent = brush.extent();
    d3.selectAll("g.brush").call(brush.clear());
    drawAxes(extent[0][0], extent[1][0], extent[0][1], extent[1][1], isPercent);
    draw();     
  }  
}

function update() {
  var variable = getVariable();
  var isPercent = getIsPercent();
  var isMovingAverage = getIsMovingAverage();
  yAxisTitle.text(isMovingAverage ? variable + " Moving Average" : variable);
  var valueRange = calculateValue(data, variable, isMovingAverage, isPercent);
  drawAxes(isMovingAverage ? movingAverageMinDate : minDate, maxDate, valueRange[0], valueRange[1], isPercent);
  draw();  
}

function parseQueryString(qs) {
  var urlQuery = {};
  qs.split('&').forEach(function(keyValues) {
    var kvSplit = keyValues.split('=');  
    var key, value;
    try {      
      key = decodeURIComponent(kvSplit[0]);
      value = decodeURIComponent(kvSplit[1]);
    } catch(err) {
      console.error(err);
    }
    urlQuery[key] = value;    
  });
  return urlQuery;
}

function timeDay(start, stop, step) {
  if (start.getMonth() !== stop.getMonth()) {
    var current = new Date(stop.getFullYear(), stop.getMonth(), 1);
    while (current >= start) current = d3.time.day.offset(current, -step);
    current = d3.time.day.offset(current, step);
    var times = [];
    while (current <= stop) {
      times.push(current);
      current = d3.time.day.offset(current, step);
    }
    return times;
  } else {
    return d3.time.day.range(start, stop, step);
  }
}

function isMobile() {
  var a = navigator.userAgent || navigator.vendor || window.opera;
  return /(android|bb\d+|meego).+mobile|avantgo|bada\/|blackberry|blazer|compal|elaine|fennec|hiptop|iemobile|ip(hone|od)|iris|kindle|lge |maemo|midp|mmp|mobile.+firefox|netfront|opera m(ob|in)i|palm( os)?|phone|p(ixi|re)\/|plucker|pocket|psp|series(4|6)0|symbian|treo|up\.(browser|link)|vodafone|wap|windows ce|xda|xiino|android|ipad|playbook|silk/i.test(a) || /1207|6310|6590|3gso|4thp|50[1-6]i|770s|802s|a wa|abac|ac(er|oo|s\-)|ai(ko|rn)|al(av|ca|co)|amoi|an(ex|ny|yw)|aptu|ar(ch|go)|as(te|us)|attw|au(di|\-m|r |s )|avan|be(ck|ll|nq)|bi(lb|rd)|bl(ac|az)|br(e|v)w|bumb|bw\-(n|u)|c55\/|capi|ccwa|cdm\-|cell|chtm|cldc|cmd\-|co(mp|nd)|craw|da(it|ll|ng)|dbte|dc\-s|devi|dica|dmob|do(c|p)o|ds(12|\-d)|el(49|ai)|em(l2|ul)|er(ic|k0)|esl8|ez([4-7]0|os|wa|ze)|fetc|fly(\-|_)|g1 u|g560|gene|gf\-5|g\-mo|go(\.w|od)|gr(ad|un)|haie|hcit|hd\-(m|p|t)|hei\-|hi(pt|ta)|hp( i|ip)|hs\-c|ht(c(\-| |_|a|g|p|s|t)|tp)|hu(aw|tc)|i\-(20|go|ma)|i230|iac( |\-|\/)|ibro|idea|ig01|ikom|im1k|inno|ipaq|iris|ja(t|v)a|jbro|jemu|jigs|kddi|keji|kgt( |\/)|klon|kpt |kwc\-|kyo(c|k)|le(no|xi)|lg( g|\/(k|l|u)|50|54|\-[a-w])|libw|lynx|m1\-w|m3ga|m50\/|ma(te|ui|xo)|mc(01|21|ca)|m\-cr|me(rc|ri)|mi(o8|oa|ts)|mmef|mo(01|02|bi|de|do|t(\-| |o|v)|zz)|mt(50|p1|v )|mwbp|mywa|n10[0-2]|n20[2-3]|n30(0|2)|n50(0|2|5)|n7(0(0|1)|10)|ne((c|m)\-|on|tf|wf|wg|wt)|nok(6|i)|nzph|o2im|op(ti|wv)|oran|owg1|p800|pan(a|d|t)|pdxg|pg(13|\-([1-8]|c))|phil|pire|pl(ay|uc)|pn\-2|po(ck|rt|se)|prox|psio|pt\-g|qa\-a|qc(07|12|21|32|60|\-[2-7]|i\-)|qtek|r380|r600|raks|rim9|ro(ve|zo)|s55\/|sa(ge|ma|mm|ms|ny|va)|sc(01|h\-|oo|p\-)|sdk\/|se(c(\-|0|1)|47|mc|nd|ri)|sgh\-|shar|sie(\-|m)|sk\-0|sl(45|id)|sm(al|ar|b3|it|t5)|so(ft|ny)|sp(01|h\-|v\-|v )|sy(01|mb)|t2(18|50)|t6(00|10|18)|ta(gt|lk)|tcl\-|tdg\-|tel(i|m)|tim\-|t\-mo|to(pl|sh)|ts(70|m\-|m3|m5)|tx\-9|up(\.b|g1|si)|utst|v400|v750|veri|vi(rg|te)|vk(40|5[0-3]|\-v)|vm40|voda|vulc|vx(52|53|60|61|70|80|81|83|85|98)|w3c(\-| )|webc|whit|wi(g |nc|nw)|wmlb|wonu|x700|yas\-|your|zeto|zte\-/i.test(a.substr(0,4));
}
