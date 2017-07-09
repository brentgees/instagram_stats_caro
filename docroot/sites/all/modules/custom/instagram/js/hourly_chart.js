/**
 * Google Chart Tools javascript
 *
 */

// Load the Visualization API and the chart package.
// google.load("visualization", "1", {packages: ["corechart", "gauge", "orgchart", "geochart"]});

(function ($) {

    $(document).ready(function () {
        chartssettings = Drupal.settings.instagram_hourly_stats;
        for (var chartId in chartssettings) {
            chartsetting = Drupal.settings.instagram_hourly_stats[chartId];
            drawHourlyChart(chartsetting);
        }
    });

    $(window).resize(function(){
        chartssettings = Drupal.settings.instagram_hourly_stats;
        for (var chartId in chartssettings) {
            chartsetting = Drupal.settings.instagram_hourly_stats[chartId];
            drawHourlyChart(chartsetting);
        }
    });

    function drawHourlyChart(chartsettings) {
        if (typeof chartsettings !== undefined) {
            // Loop on the charts in the settings.
            // Create the data table.
            var data = new google.visualization.DataTable();
            // OrgChart charts need a different format data table.

            data.addColumn('string', 'string');

            // Adding the colomns.
            // These are graphs titles.
            for (var col in chartsettings.columns) {
                data.addColumn('number', chartsettings.columns[col]);
            }

            // Adding the headers.
            // The rows titles.
            for (var i in chartsettings.header) {
                var row = new Array();
                // Adding the rows.
                // The points of the column for each row.
                for (var j in chartsettings.rows) {
                    row[j] = parseFloat(chartsettings.rows[j][i]);
                }
                row.unshift(chartsettings.header[i]);
                data.addRows([row]);
            }
            ;

            // Set chart options
            var options = chartsettings.options;

            // Instantiate and draw our chart, passing in some options.
            var chart = new Object;
            var element = document.getElementById(chartsettings.containerId);
            if (element) {
                chart[chartsettings] = new google.visualization[chartsettings.chartType](element);
                chart[chartsettings].draw(data, options);
            }
        }
    }
})(jQuery);
