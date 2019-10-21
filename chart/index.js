const { CanvasRenderService } = require('chartjs-node-canvas')
const fs = require('fs')

const width = 1920
const height = 1080

const configuration = {
  type: 'bar',
  data: {
    labels: ['Red', 'Blue', 'Yellow'],
    datasets: [{
      label: '# of Votes',
      data: [12, 19, 3],
      backgroundColor: [
        'Red',
        'Blue',
        'Yellow'
      ],
      borderColor: [
        'DarkRed',
        'DarkBlue',
        'DarkYellow'
      ],
      borderWidth: 1
    }]
  },
  options: {
    scales: {
      yAxes: [{
        ticks: {
          beginAtZero: true,
          callback: (value) => value + ' ns'
        }
      }]
    }
  }
};

const chartCallback = (ChartJS) => {

  // Global config example: https://www.chartjs.org/docs/latest/configuration/
  ChartJS.defaults.global.elements.rectangle.borderWidth = 2
  ChartJS.defaults.global.defaultFontSize = 16

  // Global plugin example: https://www.chartjs.org/docs/latest/developers/plugins.html
  ChartJS.plugins.register({
    beforeDraw: function (chart) {
      var ctx = chart.chart.ctx
      ctx.save()
      ctx.fillStyle = "#ffffff"
      ctx.fillRect(0, 0, chart.width, chart.height)
      ctx.restore()
    }
  });
  // New chart type example: https://www.chartjs.org/docs/latest/developers/charts.html
  ChartJS.controllers.MyType = ChartJS.DatasetController.extend({
    // chart implementation
  });
};

(async () => {
  const canvasRenderService = new CanvasRenderService(width, height, chartCallback)
  const image = await canvasRenderService.renderToBuffer(configuration)

  fs.writeFile("./image.png", image, function (err) {
    if (err) throw err
  });
})();