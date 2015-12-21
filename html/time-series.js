var variables = ["Contexts", "Programs", "Total Matches"];
var movingAverageWindow = 7;
var width = 800;
var height = 475;
var margin = {top: 5, right: 10, bottom: 20, left: 60};
var urlQuery = parseQueryString(window.location.search.slice(1));
var fileName = urlQuery['filename'] || 'default-time-series.csv';
var title = urlQuery['title'] || null;
if (title === null) {
  var graphTitle = document.getElementById('graph-title');
  graphTitle.parentNode.removeChild(graphTitle);
} else {
  document.getElementById('graph-title').textContent = title;
}
// graph
var controls = d3.select("#graph")
               .append("div").attr("id", "controls")
               .style("width", 960 + "px")
               .style("height", (500-height) + "px");
var legend = d3.select("#graph") 
             .append("div")
             .attr("id", "legend")
             .style("width", (960 - width) + "px")
             .style("height", height + "px")
             .style("display", "block")
             .style("float", "right");
var svg = d3.select("#graph")
          .append("svg")
          .attr("width", width)
          .attr("height", height)
          .style("display", "block");
svg.append("clipPath")
.attr("id", "clip")
.append("rect")
.attr("x", margin.left).attr("y", margin.top-5)
.attr("width", width - margin.left - margin.right + 5)
.attr("height", height - margin.top - margin.bottom + 5);
legend.append("h2").text("Legend");
legend = legend.append("ul");

// set up axes
var x = d3.time.scale().range([margin.left, width - margin.right]);
var y = d3.scale.linear().range([height - margin.bottom , margin.top]);
var xAxis = d3.svg.axis()
            .scale(x).orient("bottom")
            .innerTickSize(-height + margin.bottom + margin.top).outerTickSize(10).tickPadding(8)
            .tickFormat(d3.time.format("%b %d"));
var yAxis = d3.svg.axis()
            .innerTickSize(-width + margin.left + margin.right).outerTickSize(10).tickPadding(8)
            .scale(y).orient("left")
            .tickFormat(d3.format("%"));
svg.append("g")
.attr("class", "x axis")
.attr("transform", "translate(0," + (height - margin.bottom) + ")");
// d3.select(".x.axis")
// .append("text").attr("id", "x-axis-title")
// .attr("x", margin.left + (width - margin.right - margin.left)/2)
// .attr("y", 45).attr("text-anchor", "middle")
// .text("Date");
svg.append("g")
.attr("class", "y axis")
.attr("transform", "translate(" + margin.left + "," + 0 + ")");
var yAxisTitle = d3.select(".y.axis")
                 .append("text").attr("id", "y-axis-title")
                 .attr("x", -50)
                 .attr("y", margin.top + (height - margin.top - margin.left)/2)
                 .attr("text-anchor", "middle")
                 .attr("transform", "rotate(-90, -50," + (margin.top + (height - margin.top - margin.left)/2) + ")")
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
var isMovingAverage = controls.append("fieldset");
isMovingAverage.append("label").text("Moving Average:")
.attr("for", "isMovingAverage");
isMovingAverage.append("input")
.attr("type", "checkbox")
.attr("id", "isMovingAverage")
.attr("name", "isMovingAverage")
.on("change", update)
.node().checked = true;
var isPercentSelector = controls.append("fieldset");
isPercentSelector.append("label").text("Percentage:")
.attr("for", "isPercent");
isPercentSelector.append("input")
.attr("type", "checkbox")
.attr("id", "isPercent")
.attr("name", "isPercent")
.on("change", update)
.node().checked = true;
controls.append("button").attr("type", "button")
.text("Reset Zoom")
.on("click", brushed);
controls.append("span")
.attr("class", "info").html("Hold the <em>Control</em> key to enable tooltips.");

// set up paths
var liner = d3.svg.line()
            .x(function(d) { return x(d.Date); })
            .y(function(d) { return y(d._value); });
var strokeColor = d3.scale.category10();
// point tip
var pointTip = d3.tip()
               .attr('class', 'tip')
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

var data, minDate, maxDate, movingAverageMinDate;     //global data variables
var dailyTotals = {};
d3.csv(fileName, function(err, rawData) {
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
  data = data.slice(0, 10);
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
              .attr("width", 960-width)
              .attr("height", 30);
    svg.append("line")
    .attr("x1", 0).attr("y1", h/2)
    .attr("x2", w).attr("y2", h/2)
    .style("stroke", strokeColor(d.key));
    // svg.append("circle")
    // .attr("cx", w/2).attr("cy", h/2).attr("r", 2.5)
    // .style("fill", strokeColor(d.key));
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
  svg.append("g").attr("class", "brush").call(brush);
  svg.call(pointTip);
});

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