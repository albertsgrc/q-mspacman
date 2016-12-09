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
 *  RATEGRAPH
 *  This class is able to plot the learning curve of the agent.
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
import org.jfree.chart.axis.AxisLocation;
import org.jfree.chart.axis.NumberAxis;
import org.jfree.chart.plot.PlotOrientation;
import org.jfree.chart.plot.XYPlot;
import org.jfree.chart.renderer.xy.DeviationRenderer;
import org.jfree.chart.title.TextTitle;
import org.jfree.data.xy.XYDataset;
import org.jfree.data.xy.YIntervalSeries;
import org.jfree.data.xy.YIntervalSeriesCollection;
import org.jfree.ui.ApplicationFrame;
import org.jfree.ui.RectangleInsets;
import org.jfree.ui.RefineryUtilities;

public class RateGraph extends ApplicationFrame {

    private static YIntervalSeries series1 = null;
    private static YIntervalSeries series2 = null;
    private static YIntervalSeries series3 = null;

    public RateGraph(String title) {
        super(title);
        JPanel chartPanel = createDemoPanel();
        chartPanel.setPreferredSize(new java.awt.Dimension(680, 420));
        setContentPane(chartPanel);


        this.pack();
        RefineryUtilities.centerFrameOnScreen(this);
        this.setVisible(true);
    }

    public void addLifeTime(int epoch, double avglifetime, double maxlifetime, double minlifetime) {
        series1.add(epoch, avglifetime, minlifetime, maxlifetime);
    }

    public void addCompletion(int epoch, double avgscore, double maxscore, double minscore) {
        series2.add(epoch, avgscore, minscore, maxscore);
    }

    public void addReward(int epoch, double avgreward, double maxreward, double minreward) {
        series3.add(epoch, avgreward, minreward, maxreward);
    }

    private static XYDataset createDataset() {

        
        if (Globals.plotLifeTime) series1 = new YIntervalSeries("Lifetime (epochs)");
        series2 = new YIntervalSeries("Completion (percentage)");
        if (Globals.plotAbsoluteRewards) series3 = new YIntervalSeries("Reward (points)");

        YIntervalSeriesCollection dataset = new YIntervalSeriesCollection();
        
        if (Globals.plotLifeTime) dataset.addSeries(series1);
        dataset.addSeries(series2);
        if (Globals.plotAbsoluteRewards) dataset.addSeries(series3);

        return dataset;

    }

    private static JFreeChart createChart(XYDataset dataset) {

        // create the chart...
        JFreeChart chart = ChartFactory.createXYLineChart(
                "Learning rate", // chart title
                "Time (games)", // x axis label
                "", // y axis label
                dataset, // data
                PlotOrientation.VERTICAL,
                true, // include legend
                true, // tooltips
                false // urls
                );

        final TextTitle subtitle = new TextTitle("Running average (over " + Globals.runningAverage + " games)");
        chart.addSubtitle(subtitle);

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
        renderer.setSeriesFillPaint(0, new Color(200, 200, 255));
        renderer.setSeriesFillPaint(1, new Color(255, 200, 200));
        renderer.setSeriesFillPaint(2, new Color(200, 255, 255));
        plot.setRenderer(renderer);
        plot.setNoDataMessage("Collecting data ...");
        plot.setRangeAxisLocation(AxisLocation.TOP_OR_RIGHT);

        // change the auto tick unit selection to integer units only...
        NumberAxis yAxis = (NumberAxis) plot.getRangeAxis();
        yAxis.setStandardTickUnits(NumberAxis.createIntegerTickUnits());
        yAxis.setRange(0,100);

        NumberAxis xAxis = (NumberAxis) plot.getDomainAxis();
        xAxis.setAutoRangeIncludesZero(true);
        xAxis.setStandardTickUnits(NumberAxis.createIntegerTickUnits());

        return chart;

    }

    public static JPanel createDemoPanel() {
        JFreeChart chart = createChart(createDataset());
        return new ChartPanel(chart);
    }
    
}
