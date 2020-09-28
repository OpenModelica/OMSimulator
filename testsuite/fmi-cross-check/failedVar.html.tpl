<!DOCTYPE html>
<html>
  <head>
    <script type="text/javascript" src="../../dygraph.js"></script>
    <style type="text/css">
      #graphdiv {
      position: absolute;
      left: 10px;
      right: 10px;
      top: 40px;
      bottom: 10px;
      }
    </style>
  </head>
  <body>
    <div id="graphdiv"></div>
    <p><input type=checkbox id="0" checked onClick="change(this)">
      <label for="0">reference</label>
      <input type=checkbox id="1" checked onClick="change(this)">
      <label for="1">actual</label>
      <input type=checkbox id="2" checked onClick="change(this)">
      <label for="2">high</label>
      <input type=checkbox id="3" checked onClick="change(this)">
      <label for="3">low</label>
      <input type=checkbox id="4" checked onClick="change(this)">
      <label for="4">error</label>
      <input type=checkbox id="5" onClick="change(this)">
      <label for="5">actual (original)</label>
      Parameters used for the comparison: Relative tolerance #RelTol# (local), #MaxMin# (relative to max-min). Range delta #RangeDelta#.
    </p>
    <script type="text/javascript">
      g = new Dygraph(document.getElementById("graphdiv"),
                      "#CSVFile#",{title: "#FailedVar#",
        legend: 'always',
        connectSeparatedPoints: true,
        xlabel: ['time'],
        y2label: ['error'],
        series : { 'error': { axis: 'y2' } },
        colors: ['blue','red','teal','lightblue','orange','black'],
        visibility: [true,true,true,true,true,false]
      });
      function change(el) {
        g.setVisibility(parseInt(el.id), el.checked);
      }
    </script>
  </body>
</html>
