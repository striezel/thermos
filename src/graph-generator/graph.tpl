<!--section-start::full--><!DOCTYPE html>
<html lang="en">
{{>header}}
  <body>
    <div class="container">
    {{>content}}
    </div>
  </body>
</html>
<!--section-end::full-->

<!--section-start::header-->  <head>
    <title>{{title}}</title>
    {{>scripts}}
  </head><!--section-end::header-->

<!--section-start::script--><script src="{{path}}"></script><!--section-end::script-->

<!--section-start::graph-->
<div id="{{plotId}}"> </div>
<script>
  var traces = [];

{{>traces}}
  var layout = {
    title: '{{title}}',
    yaxis: {
       title: 'CPU load percentage'
    },
    yaxis2: {
       title: 'temperature in ° Celsius',
       overlaying: 'y',
       side: 'right'
    }
  };
  Plotly.newPlot('{{plotId}}', traces, layout, {
      displaylogo: false,
      modeBarButtonsToRemove: ['sendDataToCloud']
  });
</script>
<!--section-end::graph-->

<!--section-start::trace-->
  traces.push({
      x: {{>dates}},
      y: {{>values}},
      {{>yaxis}}
      type: 'scatter',
      mode: 'lines',
      name: '{{name}}'
  });<!--section-end::trace-->

