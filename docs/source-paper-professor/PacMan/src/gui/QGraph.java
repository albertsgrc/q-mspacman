/*  
 *  Ms. Pac-Man Framework
 * 
 *  Created by Luuk Bom & Ruud Henken under supervision by Marco Wiering,
 *  Department of Artificial Intelligence, University of Groningen.
 * 
 * 
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 
 *  QGRAPH
 *  This class is able to plot state-action values.
 * 
 */

package gui;

import global.Globals;
import java.awt.BasicStroke;
import java.awt.Color;

import javax.swing.JPanel;

import org.jfree.chart.ChartFactory;
import org.jfree.chart.ChartPanel;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.axis.NumberAxis;
import org.jfree.chart.plot.PlotOrientation;
import org.jfree.chart.plot.XYPlot;
import org.jfree.chart.renderer.xy.DeviationRenderer;
import org.jfree.data.xy.XYDataset;
import org.jfree.data.xy.YIntervalSeries;
import org.jfree.data.xy.YIntervalSeriesCollection;
import org.jfree.ui.ApplicationFrame;
import org.jfree.ui.RectangleInsets;
import org.jfree.ui.RefineryUtilities;

public class QGraph extends ApplicationFrame {

    private static YIntervalSeries series1 = null;
    private static YIntervalSeries series2 = null;
    private static YIntervalSeries series3 = null;
    private static YIntervalSeries series4 = null;
    private static int itemcount = 1;

    public QGraph(String title) {
        super(title);
        JPanel chartPanel = createDemoPanel();
        chartPanel.setPreferredSize(new java.awt.Dimension(680, 420));
        setContentPane(chartPanel);

        this.pack();
        RefineryUtilities.centerFrameOnScreen(this);
        this.setVisible(true);
    }

    public void clearData() {
    }

    public void addQValue(int net, int epoch, double value) {
        switch (net) {
            case 0:
                series1.add(epoch, value, value, value);
                break;
            case 1:
                series2.add(epoch, value, value, value);
                break;
            case 2:
                series3.add(epoch, value, value, value);
                break;
            case 3:
                series4.add(epoch, value, value, value);
                break;
        }
    }

    private static XYDataset createDataset() {

        series1 = new YIntervalSeries("Left");
        series2 = new YIntervalSeries("Right");
        series3 = new YIntervalSeries("Up");
        series4 = new YIntervalSeries("Down");

        int maxitem;
        if (Globals.showVisualisation == true) {
            maxitem = 200;
        } else {
            maxitem = 1000;
        }

        series1.setMaximumItemCount(maxitem);
        series2.setMaximumItemCount(maxitem);
        series3.setMaximumItemCount(maxitem);
        series4.setMaximumItemCount(maxitem);

        YIntervalSeriesCollection dataset = new YIntervalSeriesCollection();
        dataset.addSeries(series1);
        dataset.addSeries(series2);
        dataset.addSeries(series3);
        dataset.addSeries(series4);

        return dataset;

    }

    private static JFreeChart createChart(XYDataset dataset) {

        // create the chart...
        JFreeChart chart = ChartFactory.createXYLineChart(
                "State-action values", // chart title
                "Time (network runs)", // x axis label
                "Q-value", // y axis label
                dataset, // data
                PlotOrientation.VERTICAL,
                true, // include legend
                true, // tooltips
                false // urls
                );

        chart.setBackgroundPaint(Color.white);

        // get a reference to the plot for further customisation...
        XYPlot plot = (XYPlot) chart.getPlot();
        plot.setBackgroundPaint(Color.lightGray);
        plot.setAxisOffset(new RectangleInsets(5.0, 5.0, 5.0, 5.0));
        plot.setDomainGridlinePaint(Color.white);
        plot.setRangeGridlinePaint(Color.white);

        DeviationRenderer renderer = new DeviationRenderer(true, false);
        renderer.setSeriesStroke(0, new BasicStroke(3.0f));
        renderer.setSeriesStroke(1, new BasicStroke(3.0f));
        renderer.setSeriesStroke(2, new BasicStroke(3.0f));
        renderer.setSeriesStroke(3, new BasicStroke(3.0f));
        renderer.setSeriesFillPaint(0, new Color(200, 200, 255));
        renderer.setSeriesFillPaint(1, new Color(255, 200, 200));
        renderer.setSeriesFillPaint(0, new Color(200, 255, 200));
        renderer.setSeriesFillPaint(1, new Color(255, 255, 200));
        plot.setRenderer(renderer);

        // change the auto tick unit selection to integer units only...
        NumberAxis yAxis = (NumberAxis) plot.getRangeAxis();
        yAxis.setAutoRangeMinimumSize(3);
        yAxis.setStandardTickUnits(NumberAxis.createIntegerTickUnits());

        NumberAxis xAxis = (NumberAxis) plot.getDomainAxis();
        xAxis.setAutoRangeIncludesZero(false);
        xAxis.setStandardTickUnits(NumberAxis.createIntegerTickUnits());

        return chart;

    }

    public static JPanel createDemoPanel() {
        JFreeChart chart = createChart(createDataset());
        return new ChartPanel(chart);
    }

}
