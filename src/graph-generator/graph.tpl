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

<!--section-start::link_with_integrity--><link href="{{url}}" rel="stylesheet" integrity="{{hash}}" crossorigin="anonymous"><!--section-end::link_with_integrity-->

<!--section-start::script_with_integrity--><script src="{{url}}" integrity="{{hash}}" crossorigin="anonymous"></script><!--section-end::script_with_integrity-->

<!--section-start::script--><script src="{{url}}"></script><!--section-end::script-->

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

<!--section-start::navigation-->
<nav class="nav nav-pills nav-fill">
  {{>items}}
</nav>
<!--section-end::navigation-->

<!--section-start::nav_item-->
  <a class="nav-link{{active}}" href="{{url}}">{{name}}</a><!--section-end::nav_item-->
