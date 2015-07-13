/*global graphData */
var width = 720, height = 720;
var minSize = 10;
var maxSize = 50;
var minDistance = 75;
var maxDistance = 650;
// we'll binary search for the correct max distance later
var lowerMaxDistance = 300;
var upperMaxDistance = 1000;
var margin = maxSize;
var rightSidebar = d3.select("#graph")
                   .append("div")
                   .attr("id", "rightSidebar");
var svg = d3.select("#graph")
          .append("svg")
          .attr("width", width)
          .attr("height", height);
var drag = d3.behavior.drag()
           .origin(function(d) { return d; })
           .on("dragstart", dragstarted)
           .on("drag", dragged)
           .on("dragend", dragended);
var layoutSelector = rightSidebar.append("div")
                     .attr("id", "layout-selector");
layoutSelector.append("h2").text("Layouts");
layoutSelector.append("button")
.text("Spring Embed")
.attr('disabled', true)
.attr('class', 'layout spring-embed')
.on("click", function() {
  springEmbedLayout({ offset: true }); draw(1000);
});
layoutSelector.append("br");
layoutSelector.append("button")
.text("Random")
.on("click", function() {
  randomLayout(); draw(1000);
});
layoutSelector.append("br");
layoutSelector.append("button")
.text("Reset")
.on("click", function() {
  circularLayout(); draw(1000);
});
var timeSelector = rightSidebar.append("div")
                   .attr("id","time-selector");
timeSelector.append("h2").text("Time Period");
var timeSelectorForm = timeSelector.append("form")
                       .on("change", timeChange);
var info = rightSidebar.append("div")
           .attr("id", "info");
info.append("h2").text("Information");
info.append("p")
.text('This visualization shows the relationship between nodes. Nodes are considered to be related if they are frequently mentioned together on television. Mouseover nodes and links to see more information about the nodes and how they are related. Nodes can be dragged. ');
info.append("p")
.html('Using <span style="color:#e41a1c">Spring Embed</span> may help you see the relationship more clearly. The algorithm tries to place nodes that are frequently mentioned together close together. The algorithm works by placing a short spring between closely related nodes and a long spring between unrelated nodes. Then, the total potential energy is minimized by applying a steepest gradient descent method iteratively. One may need to click <span style="color:#e41a1c">Random</span> and <span style="color:#e41a1c">Spring Embed</span> a few times before a desirable layout is found. <span style="color:#e41a1c">Reset</span> puts everything back in a circle.');
info.append("p")
.html('By selecting a different time period, one can see how the relationships between nodes change over time.');
info.append("h2").text("Upload File");
var fileUploader = info.append("form").attr("method", "POST")
                   .attr("action",'cgi-bin/process_file')
                   .attr("enctype",'multipart/form-data');
fileUploader.append("span").text("Matrix:").attr("class", "file-label");
fileUploader.append("input")
.attr("type", "file")
.attr("name", "matrix_file")
.attr("accept", "text/plain");
fileUploader.append("br");
fileUploader.append("input")
.attr("type", "submit")
.attr("value", "Submit");

var nodeTip = d3.tip()
              .attr('class', 'tip')
              .direction('e')
              .html(function (d) {
                var formatter = d3.format("0.3f");
                var size = d.size instanceof Object ? d.size[getTimeKey()] : d.size;
                size = formatter(size);
                return d.name + '<br>'
                     + '<strong>Size:</strong> <span style="color:#e41a1c">' + size + '</span>';;
              });
var linkTip = d3.tip()
              .attr('class', 'tip')
              .offset(function() {
                return [this.getBBox().height/2, 120];
              })
              .html(function (d) {
                var formatter = d3.format("0.3f");
                return '<span style="color:#e41a1c">' + graphData.nodes[d.source].name + '</span><br>'
                     + '<span style="color:#e41a1c">' + graphData.nodes[d.target].name + '</span><br>'
                     + '<strong>Distance:</strong> <span style="color:#e41a1c">' + formatter(d.distance) + '</span>';
              });

svg.call(nodeTip);
svg.call(linkTip);

var graphData;
var maxLinkDistance, minLinkDistance;
var link = svg.selectAll(".link.data");
var node = svg.selectAll(".node.data");
var nodeLabel = svg.selectAll(".node-label.data");
var urlQuery = {};
window.location.search.slice(1).split('&').forEach(function(keyValues) {
  var kvSplit = keyValues.split('=');  
  urlQuery[kvSplit[0]] = kvSplit[1];
});
var fileName = decodeURIComponent(urlQuery['filename'] || 'default_time.json');
var title = decodeURIComponent(urlQuery['title'] || 'Graph');
document.getElementById('graph-title').innerHTML = title;
d3.json(fileName, function(err, graph) {
  graphData = graph;
  // add times
  for (var i = 0; i < graphData.times.length; ++i) {
    var timeSelectorRadio = timeSelectorForm.append("input")
                            .attr("type", "radio")
                            .attr("name", "time")
                            .attr("value", i);
    if (i === 0) timeSelectorRadio.attr("checked", true);
    timeSelectorForm.append("span").attr("class", "time-label").text(graphData.times[i].name);
    if (i !== graphData.times.length - 1) timeSelectorForm.append("br");
  }
  if (graphData.times.length === 1) timeSelector.style("display", "none");
  
  var links = initializeGraph(graph, true);
  // don't bother drawing those with max distance, which is 10?
  link = link.data(links.filter(function(d) { return d.distance < maxLinkDistance; }))
         .enter().append("line")
         .attr("class", function(d, i) {           
           return "link data " + graph.nodes[d.source].symbol + " " + graph.nodes[d.target].symbol;
         })
         .on('mouseover', linkTip.show)
         .on('mouseout', linkTip.hide);

  node = node.data(graph.nodes)
         .enter().append("circle")
         .attr("class", "node data")
         .attr("r", function(d) { return d.r; })
         .on('mouseover', function(d) { 
           var target = node.filter(function(dd) { return dd.symbol === d.symbol; });
           nodeTip.show(d, target.node());
           this.classList.add('hover');
           // svg.selectAll('line.' + d.symbol)
           // .each(function(d) {
           //   d.tip.show.call(this, d, this);
           // });           
         })
         .on('mouseout',  function(d) { 
           var target = node.filter(function(dd) { return dd.symbol === d.symbol; });
           nodeTip.hide(d, target.node());
           this.classList.remove('hover');
           // svg.selectAll('line.' + d.symbol)
           // .each(function(d) {
           //   d.tip.hide.call(this, d, this);
           // });           
         })
         .call(drag);
  nodeLabel = nodeLabel.data(graph.nodes)
              .enter().append("text")
              .attr("class", "node-label data")
              .attr("text-anchor", "middle")
              .attr("dy", "5px")
              .text(function(d) { return d.symbol; })
              .on('mouseover', function(d) {
                var target = node.filter(function(dd) { return dd.symbol === d.symbol; });
                target.node().classList.add('hover');
                nodeTip.show(d, target.node());
              })
              .on("mouseout", function(d) {
                var target = node.filter(function(dd) { return dd.symbol === d.symbol; });
                target.node().classList.remove('hover');
                nodeTip.hide(d, target.node());
              })
              .call(drag);
  circularLayout();
  draw();
});

// draw graph
function draw(duration) {
  duration = typeof duration !== 'undefined' ? duration : 0;
  link.transition().duration(duration)
  .attr("x1", function(d) { return graphData.nodes[d.source].x; })
  .attr("y1", function(d) { return graphData.nodes[d.source].y; })
  .attr("x2", function(d) { return graphData.nodes[d.target].x; })
  .attr("y2", function(d) { return graphData.nodes[d.target].y; });
  node.transition().duration(duration)
  .attr("cx", function(d) { return d.x; })
  .attr("cy", function(d) { return d.y; })
  .attr("r", function(d) { return d.r; });
  nodeLabel.transition().duration(duration)
  .attr("x", function(d) { return d.x; })
  .attr("y", function(d) { return d.y; });
}


// various layouts
function springEmbedLayout(options) {
  options = options || {};
  var nodes = graphData.nodes;
  var links = graphData.links;
  var n = nodes.length;
  function distance(a, b) {
    return Math.sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y));
  }
  function energy(nodes) {
    var E = 0;
    for (var i = 0; i < n - 1; ++i) {
      for (var j = i+1; j < n; ++j) {
        var diff = distance(nodes[i], nodes[j]) - links[i][j].l;
        E += 0.5*links[i][j].k*diff*diff;
      }
    }
    return E;
  }
  function dEd(idx, f) {
    var res = 0;
    for (var i = 0; i < n; ++i) {
      if (i === idx) continue;
      res += links[idx][i].k*((f(nodes[idx]) - f(nodes[i])) - links[idx][i].l*(f(nodes[idx]) - f(nodes[i]))/distance(nodes[idx], nodes[i]));            
    }

    return res;
  }
  function dEdx(idx) {
    return dEd(idx, function(d) { return d.x; });
  }
  function dEdy(idx) {
    return dEd(idx, function(d) { return d.y; });
  }  
  function gradient() {
    var grad = [];
    for (var i = 0; i < n; ++i) {
      grad.push({x: dEdx(i), y: dEdy(i)});
    }
    return grad;
  }
  function norm(v) {
    var n = 0;
    v.forEach(function(d) {
      n += d.x*d.x + d.y*d.y;
    });
    return Math.sqrt(n);
  }  
  function addVector(v, w, alpha) {
    var u = [];
    for (var i  = 0; i < n; ++i) {
      u.push({x: v[i].x + w[i].x*alpha, y: v[i].y + w[i].y*alpha});
    }
    return u;
  }
  function divideVector(v, alpha) {
    for (var i  = 0; i < n; ++i) {
      v[i].x /= alpha; v[i].y /= alpha;
    }
    return v;
  }
  function setVector(v, w) {
    for (var i = 0; i < n; ++i) {
      v[i].x = w[i].x; v[i].y = w[i].y;
    }
    return v;
  }
  var TOL = 0.001;
  var E = energy(nodes);
  var grad = gradient();
  var gradNorm = norm(grad);
  var done = gradNorm < TOL;
  var iterations = 0;
  while (!done) {
    divideVector(grad, gradNorm); // make a unit vector
    var alpha1 = 0;
    var alpha3 = 1;
    var E3 = energy(addVector(nodes, grad, -alpha3));
    while (E3 >= E) {
      alpha3 /= 2;
      E3 = energy(addVector(nodes, grad, -alpha3));
    }
    var alpha2 = alpha3/2;
    var E2 = energy(addVector(nodes, grad, -alpha2));
    var h1 = (E2 - E)/alpha2;
    var h2 = (E3 - E2)/(alpha3-alpha2);
    var h3 = (h2-h1)/alpha3;
    var alpha0 = (alpha2 - h1/h3)/2;
    var E0 = energy(addVector(nodes, grad, -alpha0));
    var newE, alpha;
    if (E0 < E3) {
      newE = E0; alpha = alpha0;
    } else {
      newE = E3; alpha = alpha3;
    }        
    // reset state
    if (E - newE < TOL) done = true;
    setVector(nodes, addVector(nodes, grad, -alpha));
    E = newE;
    grad = gradient();
    gradNorm = norm(grad);
    if (gradNorm < TOL) done = true;
    iterations += 1;
  }
  // recenter
  var cX = 0; 
  var cY = 0;
  for (var i = 0; i < n; ++i) {
    cX += nodes[i].x;
    cY += nodes[i].y;
  }
  cX /= n; 
  cY /= n;
  var leftOverflow = 0, topOverflow = 0, rightOverflow = 0, bottomOverflow = 0;
  nodes.forEach(function(d) {
    d.x += width/2 - cX;
    d.y += height/2 - cY;
    if (d.x - d.r < 0 && d.r - d.x > leftOverflow) leftOverflow = d.r - d.x;
    if (d.y - d.r < 0 && d.r - d.y > topOverflow) topOverflow = d.r - d.y;
    if (d.x + d.r > width && d.x + d.r - width > rightOverflow) rightOverflow = d.x + d.r - width;
    if (d.y + d.r > height && d.y + d.r - height > bottomOverflow) bottomOverflow = d.y + d.r - height;
  });
  if (options.offset) {
    var leftOffset = rightOverflow === 0 && leftOverflow;
    var topOffset = bottomOverflow === 0 && topOverflow;
    var rightOffset = leftOverflow === 0 && rightOverflow;
    var bottomOffset = topOverflow === 0 && bottomOverflow;
    nodes.forEach(function(d) {
      d.x += leftOffset - rightOffset;
      d.y += topOffset - bottomOffset;
    });
  }
  return E;
}

function randomLayout() {
  node.each(function(d) {
    d.x = margin + Math.random()*(width - 2*margin);
    d.y = margin + Math.random()*(height - 2*margin);
  });
}

function circularLayout() {
  node.each(function(d, i) {
    d.x = margin + (width-2*margin)/2*Math.cos(2*Math.PI*i/node[0].length) + (width-2*margin)/2;
    d.y = margin + (width-2*margin)/2*Math.sin(2*Math.PI*i/node[0].length) + (width-2*margin)/2;
  });
}


// drag handlers
function dragstarted(d) {
  d3.event.sourceEvent.stopPropagation();
  var target = node.filter(function(dd) { return dd.symbol === d.symbol; });
  nodeTip.hide(d, target.node());
}
function dragged(d) {
  var target = node.filter(function(dd) { return dd.symbol === d.symbol; });
  nodeTip.hide(d, target.node());
  d.x = d3.event.x;
  d.y = d3.event.y;
  draw();
}
function dragended(d) {
  var target = node.filter(function(dd) { return dd.symbol === d.symbol; });
  nodeTip.show(d, target.node());
}

function getTimeKey() {
  return graphData.times[parseInt(timeSelectorForm.selectAll("input")
                                  .filter(function() { return this.checked === true; })
                                  .node().value)].key;
}


function timeChange() {  
  initializeGraph(graphData);
  springEmbedLayout({ offset: true });
  draw(1000);
}

function chooseMaxDistance() {
  // save old layout
  var oldLayout = {};
  node.each(function(d) {
    oldLayout[d.symbol] = {};
    oldLayout[d.symbol].x = d.x;
    oldLayout[d.symbol].y = d.y;
  });  
  var lowerBound = lowerMaxDistance;
  var upperBound = upperMaxDistance;
  var distance = lowerBound + Math.floor((upperBound - lowerBound)/2);
  // binary search to find the first distance that's invalid  
  while (lowerBound < upperBound) {
    if (!isInside()) {
      upperBound = distance;
    } else {
      lowerBound = distance + 1;
    }
    var oldDistance = distance;
    distance = lowerBound + Math.floor((upperBound - lowerBound)/2);
    setIdealDistance(distance);
  } 
  setIdealDistance(distance - 1);
  maxDistance = distance - 1;
  // restore old layout
  node.each(function(d) {
    d.x = oldLayout[d.symbol].x;
    d.y = oldLayout[d.symbol].y;
  });  
  return distance;
  
  function isInside() {
    circularLayout();
    springEmbedLayout();
    return isInsideArea();
  }  
}

function isInsideArea() {
  return graphData.nodes.every(function(d) {
           return d.x >= d.r && d.x <= width - d.r && d.y >= d.r && d.y <= height - d.r;
         });
}

function setIdealDistance(maxDistance) {
  graphData.links.forEach(function(d) {
    d.forEach(function(dd) {
      if (maxLinkDistance !== minLinkDistance) {        
        dd.l = minDistance + (maxDistance - minDistance)*(dd.distance-minLinkDistance)/(maxLinkDistance-minLinkDistance);
      } else {
        dd.l = maxDistance;
      }
    });
  }); 
}


// maybe a tip for each link?
function makeLinkTip() {
  var linkTip = d3.tip()
                .attr('class', 'tip')
                .offset(function() {
                  return [this.getBBox().height/2, 120];
                })
                .html(function (d) {
                  var formatter = d3.format("0.3f");
                  return '<span style="color:#e41a1c">' + graphData.nodes[d.source].name + '</span><br>'
                       + '<span style="color:#e41a1c">' + graphData.nodes[d.target].name + '</span><br>'
                       + '<strong>Distance:</strong> <span style="color:#e41a1c">' + formatter(d.distance) + '</span>';
                });
  svg.call(linkTip);
  return linkTip;  
}


function initializeGraph(graph, makeLinks) {    
  // build link data
  var links = [];
  minLinkDistance = Infinity;
  maxLinkDistance = -1;
  var timeKey = getTimeKey();
  for (var i = 0; i < graph.nodes.length - 1; ++i) {
    for (var j = i + 1; j < graph.nodes.length; ++j) {
      if (makeLinks) links.push({source: i, target: j, distance: graphData.links[i][j][timeKey]});
      graphData.links[i][j].distance = graphData.links[j][i].distance = graphData.links[i][j][timeKey];
      if (graphData.links[i][j].distance > maxLinkDistance) maxLinkDistance = graphData.links[i][j].distance;
      if (graphData.links[i][j].distance < minLinkDistance) minLinkDistance = graphData.links[i][j].distance;
    }
  }  
  var minNodeSize = d3.min(graph.nodes, function(d) { return d.size instanceof Object ? d.size[timeKey] : d.size; })
  var maxNodeSize = d3.max(graph.nodes, function(d) { return d.size instanceof Object ? d.size[timeKey] : d.size; })
  graphData.nodes.forEach(function(d) {
    var size = d.size instanceof Object ? d.size[timeKey] : d.size;
    d.r = minSize + (maxSize-minSize)*Math.sqrt((size - minNodeSize)/(maxNodeSize-minNodeSize));
  });

  // normalize length
  setIdealDistance(maxDistance);
  setTimeout(function() {
    d3.select('button.layout.spring-embed')
    .attr('disabled', true);
    var distance = chooseMaxDistance();
    d3.select('button.layout.spring-embed')
    .attr('disabled', null);
  }, 0);
  return makeLinks ? links : link.data();
}